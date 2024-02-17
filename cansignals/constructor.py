### NORTHWESTERN FORMULA RACING 2023-24
# This Python script takes a .csv CAN DBC file and creates C++ code with signals and messages.
# by Ryan Beam, for DAQ Logger

# Imported Libraries, py -m pip install <library>
import pandas as pd
import os

# Part 1 - Classes
class CANSignal: # represents a CAN signal
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

class CANMessage: # represents a CAN Message
    def __init__(self, message_name: str, message_id: str, size: int, can_bus: str = "can_bus", txrx: str = "RX"):
        self.Message_Name = message_name
        self.Message_ID = message_id
        self.Size = size
        self.CAN_Bus = can_bus
        self.TXRX = txrx
        self.Signals: list['CANSignal'] = []

    def construct_message(self):
        assert self.Size == len(self.Signals), f"Size ({self.Size}) should be the number of signals ({len(self.Signals)})."
        message_str = f"CAN{self.TXRX}Message<{self.Size}> {self.Message_Name}{"{"}{self.CAN_Bus}, 0x{self.Message_ID}, []() {{ Serial.println(rx_message.GetLastReceiveTime()); }}, "
        for signal in self.Signals:
            message_str += f"{signal.Signal_Name}, "
        message_str = message_str.removesuffix(", ")+"};"
        return message_str

# Part 2 - C++ Code Creation (C^3)
file = os.listdir("cansignals")[0]
data = pd.read_csv(fr"cansignals/{file}")
data = data.groupby("Message ID")

RXmessages: list['CANMessage'] = []
for i,df in data:
    df = df.reset_index(drop=True)
    message_name = df.loc[0,"Message Name"]
    size = df.shape[0]
    signals = []
    for _,row in df.iterrows():
        signals.append(CANSignal(row["Signal Name"], "float", row["Start Bit"], row["Size"], row["Factor"], row["Offset"], row["Unit"]))
    message = CANMessage(message_name, i, size)
    RXmessages.append(message)
    message.Signals = signals

with open("cansignals/CAN_RX.txt","w") as f:
    message: CANMessage
    for message in RXmessages:
        f.write(f"// Message: {message.Message_Name}\n")
        for signal in message.Signals:
            f.write(f"{signal.construct_signal()}\n")
        f.write("\n")
        f.write(f"{message.construct_message()}\n")
        f.write("\n")