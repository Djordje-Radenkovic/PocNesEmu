import os
import sys


sep = 85 * '-' + '\n'
up_to = 0


def remove_ppu(l):
    temp = []

    for line in l:
        temp.append(line[:line.find("PPU")] + line[line.find("CYC"):])

    return temp


def clear():
    if sys.platform == "win32":
        os.system("cls")
    else:
        os.system("clear")


def diff(f1, f2, up_to=-1):
    clear()

    cmdd = ''
    prom = "> "

    for line_num, (gold, mine) in enumerate(zip(f1, f2)):
        if up_to != -1:
            if line_num < up_to:
                continue

        up_to = -1

        if gold != mine:
            print(sep, end='')
            print("\t\t\tDifference in Line [" + str(line_num) + "]")
            print(sep, end='')
            print("Golden log\n" + sep + gold, end='')
            print(sep, end='')
            print("My log\n" + sep + mine, end='')
            print(sep, end='')
            
            cmdd = input(prom).lower()
            while True:
                if cmdd == "exit" or cmdd == "quit":
                    return
                if cmdd == "next":
                    os.system("clear")
                    break
                if cmdd.startswith("prev"):
                    os.system("clear")
                    diff(f1, f2, up_to=line_num-1)
                    return
                if cmdd.startswith("goto"):
                    os.system("clear")
                    diff(f1, f2, up_to=int(cmdd.split(' ')[1]))
                    return
                if cmdd == "clear" or cmdd == "cls":
                    os.system("clear")

                cmdd = input(prom).lower()

def main():
    start_dir = os.path.dirname(sys.argv[0])
    print(start_dir)

    if (start_dir != ""):
        logs_dir = "logs"
    else:
        logs_dir = "../logs"

    golden_log = remove_ppu(list(open(os.path.join(logs_dir, "nestest.log"), "r")))
    my_log = remove_ppu(list(open(os.path.join(logs_dir, "cpu.log"), "r"))[1:])

    diff(golden_log, my_log)


if __name__ == "__main__" :
    main()
