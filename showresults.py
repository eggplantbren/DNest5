import apsw
import matplotlib.pyplot as plt
import numpy as np

conn = apsw.Connection(".db/dnest5.db", flags=apsw.SQLITE_OPEN_READONLY)
db = conn.cursor()

results = dict(ids=[], levels=[])
for row in db.execute("SELECT id, level FROM particles;"):
    results["ids"].append(row[0])
    results["levels"].append(row[1])
for l in results:
    results[l] = np.array(results[l])

conn.close()

plt.plot(results["ids"], results["levels"])
plt.show()

