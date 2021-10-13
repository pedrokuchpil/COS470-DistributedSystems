import argparse

def validate_number_of_lines(r, n, path):
    expected_number_of_lines = r * n 
    file = open(path)
    lines = file.readlines()

    if len(lines) == expected_number_of_lines:
        print(
            f"Número de linhas do teste com r =  {r}, com n = {n} foi correto e igual a {expected_number_of_lines}.")
    else:
        print(
            f"Número de linhas do teste com r = {r}, com n = {n} foi errôneo e igual a {len(lines)}, não {expected_number_of_lines}.")


def validate_number_of_writes(path, n, r):
    processes = {}
    file = open(path)
    lines = file.readlines()

    for line in lines:
        pid = int(line.split('|')[1])
        if processes.get(pid):
            processes[pid] = processes[pid]+1
        else:
            processes[pid] = 1
    for k,v in processes.items():
        if v != r :
            print (f"Número de escritas {v} do processo {k} não confere com o número esperado {r}")
            return 1
    print (f"Tá tudo certo, numero de escritas {v} = numero esperado {r}")
    return 0


def validate_increasing(path):
    file = open(path)
    lines = file.readlines()
    last = -1
    for l in lines:
        new = int(l.split('|')[0])
        if new < last:
            print(f"Os valores não estão respeitando a ordem cronológica do sistema. linha")
            return
        last = new
    print(f"Os valores estão respeitando a ordem cronológica do sistema.")


def validate_execution(path):
    file = open(path)
    lines = file.readlines()
    requests = []
    last_granted = 0
    for l in lines:
        splt = l.split(' ')
        if splt[1] == '2\n':
            last_granted = int(splt[0])
        elif splt[1] == '1\n':
            requests.append(int(splt[0]))
        else:
            if int(splt[0]) != requests[0] or int(splt[0]) != last_granted:
                print(f"O coordenador apresentou um problema de concorrência {l}")
                return
            requests = requests[1:]
    print("Não ocorreram problemas de concorrência pelo coordenador.")





if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Process some integers.')
    parser.add_argument('r', type=int, help='r')
    parser.add_argument('n', type=int, help='n')
    parser.add_argument('coordpath', help='coordpath')
    parser.add_argument('resultpath', help='resultpath')
    args = parser.parse_args()

    r = args.r
    n = args.n
    coordpath = args.coordpath
    resultpath = args.resultpath

    validate_number_of_lines(r, n, resultpath)
    validate_number_of_writes(resultpath, n, r)
    validate_increasing(resultpath)
    validate_execution(coordpath)

