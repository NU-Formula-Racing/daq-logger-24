import pandas as pd
from pprint import pprint
import os

class RXSignal:
    def __init__(self, signal_name: str, signal_type: str, position: int, length: int, factor: float, offset: int, unit: str, sign: str = "Signed"):
        if sign.lower() == "signed":
            self.Make = "MakeSignedCANSignal"
        elif sign.lower() == "unsigned":
            self.Make = "MakeUnsignedCANSignal"
        else:
            raise ValueError('Type must be either "signed" or "unsigned".')
        self.Signal_Name = signal_name
        self.Signal_Type = signal_type
        self.Position = position
        self.Length = length
        self.Factor = factor
        self.Offset = offset
        self.Unit = unit

    def construct_signal(self):
        return f"{self.Make}({self.Signal_Type}, {self.Position}, {self.Length}, {self.Factor}, {self.Offset}) {self.Signal_Name}{{}};"

class RXMessage:
    def __init__(self, message_name: str, message_id: str, size: int, can_bus: str = "can_bus"):
        self.Message_Name = message_name
        self.Message_ID = message_id
        self.Size = size
        self.CAN_Bus = can_bus
        self.Signals: list['RXSignal'] = []

    def construct_message(self):
        assert self.Size == len(self.Signals), f"Size ({self.Size}) should be the number of signals ({len(self.Signals)})."
        message_str = f"CANRXMessage<{self.Size}> {self.Message_Name}{"{"}{self.CAN_Bus}, 0x{self.Message_ID}, []() {{ Serial.println(rx_message.GetLastReceiveTime()); }}, "
        for signal in self.Signals:
            message_str += f"{signal.Signal_Name}, "
        message_str = message_str.removesuffix(", ")+"};"
        return message_str

file = os.listdir("cansignals")[0]
data = pd.read_csv(fr"cansignals/{file}")
data = data.groupby("Message ID")

RXmessages: list['RXMessage'] = []
for i,df in data:
    df = df.reset_index(drop=True)
    message_name = df.loc[0,"Message Name"]
    size = df.shape[0]
    signals = []
    for _,row in df.iterrows():
        signals.append(RXSignal(row["Signal Name"], "float", row["Start Bit"], row["Size"], row["Factor"], row["Offset"], row["Unit"]))
    message = RXMessage(message_name, i, size)
    RXmessages.append(message)
    message.Signals = signals

with open("cansignals/CAN_RX.txt","w") as f:
    message: RXMessage
    for message in RXmessages:
        f.write(f"// Message: {message.Message_Name}\n")
        for signal in message.Signals:
            f.write(f"{signal.construct_signal()}\n")
        f.write("\n")
        f.write(f"{message.construct_message()}\n")
        f.write("\n")