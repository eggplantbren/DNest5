import apsw
import numpy as np
import subprocess
import showresults

logz_estimates = []
f = open(".test_accuracy_output.csv", "w")
f.write("reps,logz_estimate,rms_error\n")
f.flush()
for i in range(1000):
    subprocess.run("./main")
    conn = apsw.Connection(".db/dnest5.db", flags=apsw.SQLITE_OPEN_READONLY)
    db = conn.cursor()
    logz_estimates.append(showresults.postprocess(db)["logz"])
    f.write(str(i+1) + ",")
    f.write(str(logz_estimates[-1]) + ",")
    rms_error = np.sqrt(np.mean((np.array(logz_estimates) - np.log(101.0))**2))
    f.write(str(rms_error) + "\n")
    f.flush()
    conn.close()
