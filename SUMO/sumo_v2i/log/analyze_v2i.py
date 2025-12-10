#!/usr/bin/env python3
import sys
import re
from statistics import mean

if len(sys.argv) != 3:
    print("Uso: python3 analyze_v2i_plus.py gps.log ldm.log")
    sys.exit(1)

file_send = sys.argv[1]   # Log de GPS
file_recv = sys.argv[2]   # Log de LDM

# GPS (METRIC_TRIGGER_SEND)

send_times = {}
send_order = []
send_pattern = re.compile(r"METRIC_TRIGGER_SEND\D+(\d+)\D+(\d+)")

with open(file_send) as f:
    for line in f:
        m = send_pattern.search(line)
        if m:
            msg_id = int(m.group(1))
            t_send = int(m.group(2))
            send_times[msg_id] = t_send
            send_order.append(msg_id)

# LDM (METRIC_DENM_RCV)

recv_times = {}
recv_counts = {}   # para detectar duplicados
recv_pattern = re.compile(r"METRIC_DENM_RCV\D+(\d+)\D+(\d+)")

with open(file_recv) as f:
    for line in f:
        m = recv_pattern.search(line)
        if m:
            msg_id = int(m.group(1))
            t_recv = int(m.group(2))
            recv_times[msg_id] = t_recv
            recv_counts[msg_id] = recv_counts.get(msg_id, 0) + 1

# Emparejar triggers - DENM recibidos

latencias = []
recibidos = []
perdidos = []

for msg_id in send_order:
    if msg_id in recv_times:
        latency = recv_times[msg_id] - send_times[msg_id]
        if latency >= 0:
            latencias.append(latency)
            recibidos.append(msg_id)
        else:
            print(f"[WARN] Latencia negativa ignorada para ID {msg_id}")
    else:
        perdidos.append(msg_id)

extra_denm = sorted(set(recv_times.keys()) - set(send_times.keys()))
duplicados = sorted([msg_id for msg_id, count in recv_counts.items() if count > 1])

# Resultados

total_enviados = len(send_order)
total_recibidos_match = len(recibidos)
total_denm_raw = sum(recv_counts.values())

pdr = (total_recibidos_match / total_enviados * 100) if total_enviados > 0 else 0

print("\n===== RESULTADOS V2I (DENM) — ANALISIS DETALLADO =====\n")

print(f"Triggers enviados (GPS):        {total_enviados}")
print(f"DENM recibidos en LDM (total):  {total_denm_raw}")
print(f"DENM emparejados (válidos):     {total_recibidos_match}")
print(f"DENM no emparejados:            {len(extra_denm)}")
print(f"Triggers perdidos:              {len(perdidos)}")
print(f"Mensajes duplicados detectados: {len(duplicados)}")
print(f"PDR (solo mensajes válidos):    {pdr:.2f}%")

print("\n--- Latencias (solo DENM válidos) ---")
if latencias:
    print(f"Latencia media:  {mean(latencias):.2f} ms")
    print(f"Latencia mínima: {min(latencias)} ms")
    print(f"Latencia máxima: {max(latencias)} ms")
else:
    print("No hay latencias válidas para analizar.")

print("\n--- IDs no emparejados (DENM extra) ---")
print(extra_denm)

print("\n--- IDs de triggers perdidos ---")
print(perdidos)

print("\n--- IDs duplicados en LDM ---")
print(duplicados)

print("\n======================================================\n")
