import subprocess
import matplotlib.pyplot as plt

ERROR_PROBABILITIES = [0.05 * i for i in range(21)]
NOISE = "./noise"
ENCODER = "./encoder"
DECODER = "./decoder"
COMPARE = "./compare"

INPUT = "../tests/pan-tadeusz-czyli-ostatni-zajazd-na-litwie.txt"
NOISE_INPUT = "noised.txt"
INPUT_ENCODED = "encoded.txt"
INPUT_ENCODED_NOISED = "encoded_noised.txt"
INPUT_DECODED = "decoded.txt"

values1 = []
values2 = []
values3 = []

for p in ERROR_PROBABILITIES:
    subprocess.run([NOISE, str(p), INPUT, NOISE_INPUT]) 
    res = subprocess.check_output([COMPARE, INPUT, NOISE_INPUT])
    values1.append(float(res))

    subprocess.run([ENCODER, INPUT, INPUT_ENCODED])
    subprocess.run([NOISE, str(p), INPUT_ENCODED, INPUT_ENCODED_NOISED])
    res = subprocess.check_output([DECODER, INPUT_ENCODED_NOISED, INPUT_DECODED])
    values2.append(float(res))

    res = subprocess.check_output([COMPARE, INPUT, INPUT_DECODED])
    values3.append(float(res))

plt.plot(ERROR_PROBABILITIES, values1, label="1")
plt.plot(ERROR_PROBABILITIES, values2, label="2")
plt.plot(ERROR_PROBABILITIES, values3, label="3")
plt.xlabel("Prawdopodobieństwo błędu")
plt.ylabel("Procent błędów")
plt.savefig("results.png")
