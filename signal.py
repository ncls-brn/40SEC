import signal
import time


def signal_handler(signum, frame):
    print(f"Signal ID reçu: {signum}, Frame: {frame}")

signal.signal(signal.SIGINT, signal_handler)

time.sleep(5)



