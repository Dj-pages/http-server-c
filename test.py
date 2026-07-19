import asyncio
import aiohttp
import time
import random

BASE_URL = "http://localhost:8080"   # change port if needed
DURATION = 60
CONCURRENCY = 100

# just list the actual GET endpoints your server has
ENDPOINTS = [
    {"path": "/", "weight": 1},
    {"path": "/about.html", "weight": 1},
    {"path": "/index.html", "weight": 1},
    # add/remove to match your real routes
]

pool = []
for ep in ENDPOINTS:
    pool.extend([ep] * ep["weight"])

stats = {ep["path"]: {"count": 0, "errors": 0, "latencies": []} for ep in ENDPOINTS}

async def worker(session, stop_time):
    while time.time() < stop_time:
        ep = random.choice(pool)
        url = BASE_URL + ep["path"]
        start = time.time()
        try:
            async with session.get(url) as resp:
                await resp.read()
                latency = time.time() - start
                stats[ep["path"]]["latencies"].append(latency)
                if resp.status < 400:
                    stats[ep["path"]]["count"] += 1
                else:
                    stats[ep["path"]]["errors"] += 1
        except Exception:
            stats[ep["path"]]["errors"] += 1

async def main():
    stop_time = time.time() + DURATION
    connector = aiohttp.TCPConnector(limit=CONCURRENCY)
    async with aiohttp.ClientSession(connector=connector) as session:
        tasks = [worker(session, stop_time) for _ in range(CONCURRENCY)]
        await asyncio.gather(*tasks)

start = time.time()
asyncio.run(main())
elapsed = time.time() - start

total = sum(s["count"] for s in stats.values())
total_errors = sum(s["errors"] for s in stats.values())

print(f"\n{'='*60}")
print(f"Duration: {elapsed:.2f}s | Concurrency: {CONCURRENCY}")
print(f"Total requests: {total} | Errors: {total_errors}")
print(f"Requests/min: {(total/elapsed)*60:.2f}")
print(f"{'='*60}\n")

for path, s in stats.items():
    if s["latencies"]:
        avg_lat = sum(s["latencies"]) / len(s["latencies"]) * 1000
        p95 = sorted(s["latencies"])[int(len(s["latencies"])*0.95)] * 1000
    else:
        avg_lat = p95 = 0
    print(f"{path:15s} | reqs: {s['count']:5d} | errors: {s['errors']:4d} | avg: {avg_lat:6.1f}ms | p95: {p95:6.1f}ms")
