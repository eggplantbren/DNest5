import matplotlib.pyplot as plt
import sys
import yaml

f = open("config.yaml")
dnest5_dir = yaml.load(f, Loader=yaml.SafeLoader)["dnest5_dir"]
f.close()

if __name__ == "__main__":
    sys.path.insert(0, dnest5_dir)
    import showresults as sr
    sr.standard_results(sys.argv)

