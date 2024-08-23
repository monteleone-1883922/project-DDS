import json
import os
import sys
import numpy as np
import matplotlib.pyplot as plt


def merge_results(dir: str):
    rounds_to_decide = []
    round_times = []
    infected_round_times = []
    correct_round_times = []
    cured_round_times = []
    mean_time_all_rounds = []
    rounds_means_times = []
    final_results = {}

    for file in os.listdir("results/" + dir):
        if file.endswith(".json"):
            with open("results/" + dir + "/" + file, 'r') as f:
                results = json.load(f)
            os.remove("results/" + dir + "/" + file)
            rounds_to_decide.append(results["rounds_to_decide"])

            correct_process = results["correct_process"]
            if "num_infected" not in final_results:
                final_results["num_infected"] = results["num_infected"]
            if "num_processes" not in final_results:
                final_results["num_processes"] = results["num_processes"]
            rounds = results["rounds"]
            mean_time_round = []
            for j, round in enumerate(rounds):
                for infected in round["infected"]:
                    infected_round_times.append(round["round_time"][infected])
                if "cured" in round:
                    for cured in round["cured"]:
                        cured_round_times.append(round["round_time"][cured])
                correct_round_times.append(round["round_time"][correct_process])
                round_times += round["round_time"]
                mean_time_round.append(sum(round["round_time"]) / len(round["round_time"]))
            mean_time_all_rounds.append(mean_time_round)

    for i in range(len(mean_time_all_rounds[0])):
        rounds_means_times.append(np.mean([round[i] for round in mean_time_all_rounds]))

    final_results["mean_rounds_to_decide"] = np.mean(rounds_to_decide)
    final_results["max_rounds_to_decide"] = np.max(rounds_to_decide)
    final_results["min_rounds_to_decide"] = np.min(rounds_to_decide)
    final_results["var_rounds_to_decide"] = np.var(rounds_to_decide)

    final_results["mean_round_times"] = np.mean(round_times)
    final_results["max_round_times"] = np.max(round_times)
    final_results["min_round_times"] = np.min(round_times)
    final_results["var_round_times"] = np.var(round_times)

    final_results["mean_infected_round_times"] = np.mean(infected_round_times)
    final_results["max_infected_round_times"] = np.max(infected_round_times)
    final_results["min_infected_round_times"] = np.min(infected_round_times)
    final_results["var_infected_round_times"] = np.var(infected_round_times)

    final_results["mean_correct_round_times"] = np.mean(correct_round_times)
    final_results["max_correct_round_times"] = np.max(correct_round_times)
    final_results["min_correct_round_times"] = np.min(correct_round_times)
    final_results["var_correct_round_times"] = np.var(correct_round_times)

    final_results["mean_cured_round_times"] = np.mean(cured_round_times)
    final_results["max_cured_round_times"] = np.max(cured_round_times)
    final_results["min_cured_round_times"] = np.min(cured_round_times)
    final_results["var_cured_round_times"] = np.var(cured_round_times)

    with open(f"results/{dir}_merged_results.json", 'w') as f:
        json.dump(final_results, f, indent=4)

    plt.figure(figsize=(10, 6))
    for i, mean_time_round in enumerate(mean_time_all_rounds):
        plt.plot(mean_time_round, label=f'Run {i + 1}')
    plt.xlabel('Round')
    plt.ylabel('Mean Time')
    plt.title('Mean Time per Round for All Runs')
    plt.legend()
    plt.grid(True)
    plt.show()


def main():
    dir = sys.argv[1]


if __name__ == '__main__':
    main()
