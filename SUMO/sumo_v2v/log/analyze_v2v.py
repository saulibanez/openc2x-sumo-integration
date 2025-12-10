import sys
import re
from statistics import mean

if len(sys.argv) != 3:
    print("Uso: python3 analyze_v2v.py cam.log ldm.log")
    sys.exit(1)

file_cam = sys.argv[1]      # Log de CAM
file_ldm = sys.argv[2]      # Log de LDM

# CAM (METRIC_CAM_SEND)

send_times = {}
send_order = []
pattern_send = re.compile(r"METRIC_CAM_SEND\s+(\d+)\s+(\d+)")

with open(file_cam) as f:
    for line in f:
        match = pattern_send.search(line)
        if match:
            cam_id = int(match.group(1))
            t_send = int(match.group(2))
            send_times[cam_id] = t_send
            send_order.append(cam_id)

# LDM (METRIC_CAM_RECV)

recv_times = {}
pattern_recv = re.compile(r"METRIC_CAM_RECV\s+(\d+)\s+(\d+)")

with open(file_ldm) as f:
    for line in f:
        match = pattern_recv.search(line)
        if match:
            cam_id = int(match.group(1))
            t_recv = int(match.group(2))
            recv_times[cam_id] = t_recv

# Métricas

latencias = []
recibidos = []
perdidos = []

for cam_id in send_order:
    if cam_id in recv_times:
        t_send = send_times.get(cam_id, None)
        t_recv = recv_times.get(cam_id, None)

        if t_send is not None and t_recv is not None:
            latency = t_recv - t_send
            if latency >= 0:           # filtro robusto
                latencias.append(latency)
                recibidos.append(cam_id)
            else:
                print(f"[WARN] Latencia negativa ignorada en CAM {cam_id}")
        else:
            perdidos.append(cam_id)
    else:
        perdidos.append(cam_id)

total_enviados = len(send_order)
total_recibidos = len(recibidos)
pdr = (total_recibidos / total_enviados * 100) if total_enviados > 0 else 0

# Intervalos entre CAMs válidos

intervalos = []

# Convertir en lista ordenada por tiempo real de envío
ordered_by_time = sorted(send_times.items(), key=lambda x: x[1])
ordered_ids = [cid for cid, t in ordered_by_time]

for i in range(1, len(ordered_ids)):
    prev_id = ordered_ids[i-1]
    curr_id = ordered_ids[i]
    dt = send_times[curr_id] - send_times[prev_id]

    if dt > 0:          # evitar negativos
        intervalos.append(dt)

# Resultados

print("\n===== RESULTADOS V2V =====\n")

print(f"Total CAM enviados: {total_enviados}")
print(f"Total CAM recibidos: {total_recibidos}")
print(f"Total CAM perdidos: {len(perdidos)}")
print(f"PDR (Packet Delivery Ratio): {pdr:.2f}%")

if latencias:
    print(f"\nLatencia media: {mean(latencias):.2f} ms")
    print(f"Latencia mínima: {min(latencias)} ms")
    print(f"Latencia máxima: {max(latencias)} ms")

if intervalos:
    print(f"\nIntervalo medio entre CAMs: {mean(intervalos):.2f} ms")
    print(f"Intervalo mínimo: {min(intervalos)} ms")
    print(f"Intervalo máximo: {max(intervalos)} ms")
else:
    print("\n(No hay intervalos válidos para calcular)")

print("\nCAMs perdidos:", perdidos)
