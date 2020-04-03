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


def print_diff(line1, line2, line_num):
    clear()
    print(sep, end='')
    print("\t\t\tDifference in Line [" + str(line_num) + "]")
    print(sep, end='')
    print("Golden log\n" + sep + line1, end='')
    print(sep, end='')
    print("My log\n" + sep + line2, end='')
    print(sep, end='')


def print_help():
    print("Commands: exit/quit; next; prev; goto <line number>; clear/cls; help/?")


def diff(f1, f2):
    passed = True

    cmdd = ''
    prom = "> "

    for line_num, (gold, mine) in enumerate(zip(f1, f2)):
        if gold != mine:
            passed = False
            print_diff(gold, mine, line_num)
            
            cmdd = input(prom).lower()
            while True:
                if cmdd == "exit" or cmdd == "quit":
                    clear()
                    return
                if cmdd == "next":
                    line_num += 1
                    print_diff(f1[line_num], f2[line_num], line_num)
                if cmdd.startswith("prev"):                    
                    line_num -= 1
                    print_diff(f1[line_num], f2[line_num], line_num)
                if cmdd.startswith("goto"):
                    line_num = int(cmdd.split(' ')[1])
                    print_diff(f1[line_num], f2[line_num], line_num)
                if cmdd == "clear" or cmdd == "cls":
                    clear()
                if cmdd == "help" or cmdd == "?":
                    print_help()

                cmdd = input(prom).lower()
    
    return passed


def main():
    start_dir = os.path.dirname(sys.argv[0])

    if (start_dir != ""):
        logs_dir = "logs"
    else:
        logs_dir = "../logs"

    golden_log = remove_ppu(list(open(os.path.join(logs_dir, "nestest.log"), "r"))[:-1])
    my_log = remove_ppu(list(open(os.path.join(logs_dir, "cpu.log"), "r"))[1:])

    if diff(golden_log, my_log):
        print("PASS")


if __name__ == "__main__" :
    main()
