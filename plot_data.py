import matplotlib
matplotlib.use('TKAgg', warn=False, force=True)
import matplotlib.pyplot as plt
import argparse


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("-f", dest="filename")
    parser.add_argument("-m", dest="type")
    args = parser.parse_args()
    with open(args.filename, "r") as file:
        content = file.read()
    content = content.split("\n")[:-1]

    packet_size = [int(line.split()[0]) for line in content]
    value = [float(line.split()[1]) for line in content]

    plt.plot(packet_size, value, '-b', label=f"{args.type} dependency from packet size")
    plt.xlabel('Packet Sizes')
    plt.ylabel(f"{args.type}")
    plt.legend()
    plt.title(f"Link between packet sizes and {args.type}")
    plt.show()