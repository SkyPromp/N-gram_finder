data = []

with open("../../../Desktop/del/output.csv", "r") as f:
    header = f.readline()
    for line in f:
        line = line.strip()
        data += [line.split(";")]

data.sort(key=lambda d: d[2], reverse=True)

with open("../../../Desktop/del/output_sorted.csv", "w") as f:
    f.write(header)

    for el in data:
        f.write(";".join(el))
        f.write("\n")

    
