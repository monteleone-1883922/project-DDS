import json
import os
import sys
import pdb


def merge_results(run_name: str, num_rounds):
    num_rounds += 1
    next_file_exists = True
    i = 0
    rounds = [{} for i in range(num_rounds)]
    rounds_to_decide = None
    correct_process = None
    num_processes = None
    num_infected = None
    while next_file_exists:
        #if run_name == "seed-46_infected-1_speed-1_numProc-7":
            #breakpoint()
            #pass
        file_path = "results/results_{}.json".format(i)
        #breakpoint()
        with open(file_path, 'r') as f:
            results = json.load(f)

        rounds_to_decide = results["rounds_to_decide"] \
            if rounds_to_decide is None and "rounds_to_decide" in results \
            else rounds_to_decide
        correct_process = results[
            "correct_process"] if correct_process is None and "correct_process" in results else correct_process
        num_processes = results[
            "num_processes"] if num_processes is None and "num_processes" in results else num_processes
        num_infected = results["num_infected"] if num_infected is None and "num_infected" in results else num_infected
        if len(results["rounds"]) != num_rounds:
            with open("results/error.txt", 'a') as f:
            
                f.write("Error: Different number of rounds in the results\n" + run_name + "\n")
                f.write(file_path + "\n")
                f.write(str(num_rounds) + "\n")
                f.write(str(len(results["rounds"])) + "\n")
        for j, round in enumerate(results["rounds"]):
            if round["infected"]:
                rounds[j]["infected"] = rounds[j].get("infected", []) + [i]
            elif "cured" in round and round["cured"]:
                rounds[j]["cured"] = rounds[j].get("cured", []) + [i]
            rounds[j]["round_time"] = rounds[j].get("round_time", []) + [round["round_time"]]
        if len(rounds) != num_rounds:
            with open("results/error.txt", 'a') as f:
            
                f.write("Error: Different number of rounds in the results generated\n" + run_name + "\n")
                f.write(file_path + "\n")
                f.write(str(num_rounds) + "\n")
                f.write(str(len(rounds)) + "\n")
        next_file_exists = os.path.exists("results/results_{}.json".format(i + 1))
        i += 1

    if rounds_to_decide is None:
        with open("results/error.txt", 'a') as f:
            f.write("Error: rounds_to_decide is None\n" + run_name + "\n")
    result = {
        "run_name": run_name,
        "rounds_to_decide": rounds_to_decide,
        "correct_process": correct_process,
        "num_processes": num_processes,
        "num_infected": num_infected,
        "rounds": rounds,
    }


    with open(f"results/merged/{run_name}_merged_results.json", 'w') as f:
        json.dump(result, f, indent=4)


if __name__ == "__main__":
    run_name = sys.argv[1]
    num_rounds = int(sys.argv[2])
    merge_results(run_name, num_rounds)
