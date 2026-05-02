#!/usr/bin/env python3
"""Lossy UDP proxy for exercising the RESEND branch on loopback.

Two unidirectional proxies run in one process:

  * Forward direction:  listen-fwd  -> target-fwd  (sender -> receiver path)
  * Backward direction: listen-back -> target-back (receiver -> sender path)

Each direction independently drops packets at --drop-rate with its own seeded
RNG so runs are deterministic.
"""

import argparse
import random
import socket
import sys
import threading


def forward(listen_port, target_host, target_port, drop_rate, seed, label):
    rng = random.Random(seed)
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    if hasattr(socket, "SO_REUSEPORT"):
        try:
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
        except OSError:
            pass
    sock.bind(("127.0.0.1", listen_port))

    sent = dropped = 0
    while True:
        data, _ = sock.recvfrom(65536)
        if rng.random() < drop_rate:
            dropped += 1
            if (dropped + sent) % 100 == 0:
                print(f"[{label}] sent={sent} dropped={dropped}", file=sys.stderr)
            continue
        sock.sendto(data, (target_host, target_port))
        sent += 1


def main():
    p = argparse.ArgumentParser()
    p.add_argument("--listen-fwd",  type=int, required=True,
                   help="Listen port for the sender->receiver direction")
    p.add_argument("--target-fwd",  type=int, required=True,
                   help="Where to forward sender->receiver traffic")
    p.add_argument("--listen-back", type=int, required=True,
                   help="Listen port for the receiver->sender direction")
    p.add_argument("--target-back", type=int, required=True,
                   help="Where to forward receiver->sender traffic")
    p.add_argument("--drop-rate",   type=float, default=0.1,
                   help="Per-packet drop probability in each direction")
    p.add_argument("--seed",        type=int, default=42)
    args = p.parse_args()

    if not 0.0 <= args.drop_rate < 1.0:
        sys.exit("--drop-rate must be in [0.0, 1.0)")

    threading.Thread(
        target=forward,
        args=(args.listen_fwd,  "127.0.0.1", args.target_fwd,
              args.drop_rate, args.seed,     "fwd"),
        daemon=True,
    ).start()
    threading.Thread(
        target=forward,
        args=(args.listen_back, "127.0.0.1", args.target_back,
              args.drop_rate, args.seed + 1, "back"),
        daemon=True,
    ).start()

    threading.Event().wait()  # block forever; killed by parent


if __name__ == "__main__":
    main()
