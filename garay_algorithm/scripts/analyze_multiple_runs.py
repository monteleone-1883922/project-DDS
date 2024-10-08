import json
import os
import sys
import numpy as np
import matplotlib.pyplot as plt


def merge_results(dir: str, show_results: bool):
    experiments = ''
    rounds_to_decide = []
    round_times = []
    infected_round_times = []
    correct_round_times = []
    cured_round_times = []
    mean_time_all_rounds = []
    rounds_means_times = []
    final_results = {}

    try:
        for file in os.listdir("results/" + dir):
            if file.endswith(".json"):
                experiments = file[file.find("_")+1:-20] if experiments == "" else experiments

                with open("results/" + dir + "/" + file, 'r') as f:
                    results = json.load(f)

                os.remove("results/" + dir + "/" + file)
                if results["rounds_to_decide"] is None:
                    print(file)
                if results["rounds_to_decide"] is None:
                    with open(f"results/experiments/error_file.json", 'w') as f:
                        json.dump(results, f, indent=4)
                rounds_to_decide.append(results["rounds_to_decide"])

                correct_process = results["correct_process"]
                if "num_infected" not in final_results:
                    final_results["num_infected"] = results["num_infected"]
                if "num_processes" not in final_results:
                    final_results["num_processes"] = results["num_processes"]
                rounds = results["rounds"]
                mean_time_round = []
                for j, round in enumerate(rounds):
                    if round:
                        for infected in round["infected"]:
                            infected_round_times.append(round["round_time"][infected])
                        if "cured" in round:
                            for cured in round["cured"]:
                                cured_round_times.append(round["round_time"][cured])
                        correct_round_times.append(round["round_time"][correct_process])
                        round_times += round["round_time"]
                        mean_time_round.append(sum(round["round_time"]) / len(round["round_time"]))
                mean_time_all_rounds.append(mean_time_round)

        file = ''
        tmp_rounds_times = []
        for process_round_times in mean_time_all_rounds:
            for id, time in enumerate(process_round_times):
                if len(tmp_rounds_times) <= id:
                    tmp_rounds_times.append([time])
                else:
                    tmp_rounds_times[id].append(time)

        for times in tmp_rounds_times:
            rounds_means_times.append(np.mean(times))

        final_results["mean_rounds_to_decide"] = np.mean(rounds_to_decide)
        final_results["max_rounds_to_decide"] = int(np.max(rounds_to_decide))
        final_results["min_rounds_to_decide"] = int(np.min(rounds_to_decide))
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
        if len(cured_round_times) != 0:
            final_results["mean_cured_round_times"] = np.mean(cured_round_times)
            final_results["max_cured_round_times"] = np.max(cured_round_times)
            final_results["min_cured_round_times"] = np.min(cured_round_times)
            final_results["var_cured_round_times"] = np.var(cured_round_times)
    except Exception as e:
        with open(f"results/{dir}/errors.txt", 'w') as f:
            f.write(file + '\n')
            f.write(str(e) + '\n')
            f.write('rounds_to_decide' + str(rounds_to_decide) + '\n')
            f.write('round_times' + str(round_times) + '\n')
            f.write('infected_round_times' + str(infected_round_times) + '\n')
            f.write('correct_round_times' + str(correct_round_times) + '\n')
            f.write('cured_round_times' + str(cured_round_times) + '\n')
        with open(f"results/{dir}/errors.json", 'w') as f:
            json.dump(results, f, indent=4)

    final_results["rounds_means_times"] = rounds_means_times
    with open(f"results/experiments/{experiments}.json", 'w') as f:
        json.dump(final_results, f, indent=4)
    if show_results:
        plt.figure(figsize=(10, 6))

        plt.plot(rounds_means_times, label='times through rounds')
        plt.xlabel('Round')
        plt.ylabel('Mean Time')
        plt.title('Mean Time per Round for All Runs')
        plt.legend()
        plt.grid(True)
        plt.show()


def main():
    dir = sys.argv[1]
    show_results = len(sys.argv) > 2
    merge_results(dir, show_results)


if __name__ == '__main__':
    main()
