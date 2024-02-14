import pandas as pd

data = pd.read_csv("CAN_dbc.csv")

RX_messages = []
for i,row in data.iterrows():
    size = row["Size"]
    RX_message = f"CANRXmessage<{size}> "
    RX_messages.append(RX_message)