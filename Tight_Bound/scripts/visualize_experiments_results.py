import sys
import os
import json
import matplotlib.pyplot as plt
import re

EXPERIMENTS_DIR = 'results/experiments'


def estrai_numero(file_name, titolo):
    # Crea un pattern per trovare il titolo e il numero associato
    pattern = rf"{titolo}-(\d+)"

    # Cerca il pattern nel nome del file
    match = re.search(pattern, file_name)

    if match:
        # Restituisce il numero trovato
        return match.group(1)
    else:
        return None


def visualize_results(visualization):
    if "experiment" in visualization:
        with open(f"results/{visualization['experiment']}.json", 'r') as f:
            results = json.load(f)
        rounds_means_times = results["rounds_means_times"]
        plt.figure(figsize=(10, 6))

        plt.plot(rounds_means_times, label='times through rounds')
        plt.xlabel('Round')
        plt.ylabel('Mean Time')
        plt.title('Mean Time per Round for All Runs')
        plt.legend()
        plt.grid(True)
        plt.show()
    else:
        num_infected = visualization['numInfected'] if 'numInfected' in visualization else None
        num_processes = visualization['numProcesses'] if 'numProcesses' in visualization else None
        infection_speed = visualization['infectionSpeed'] if 'infectionSpeed' in visualization else None
        x_label = 'numInfected' if num_infected is None else 'numProcesses' if num_processes is None else 'infectionSpeed'
        title = 'infected' if num_infected is None else 'numProc' if num_processes is None else 'speed'
        fields_data = {}

        # Collect data from all JSON files in the experiments directory
        for file in os.listdir(EXPERIMENTS_DIR):
            if file.endswith(".json") \
                    and (num_infected is None or f'infected-{num_infected}' in file) \
                    and (num_processes is None or f'numProc-{num_processes}' in file) \
                    and (infection_speed is None or f'speed-{infection_speed}' in file):
                num_experiment = int(estrai_numero(file, title))
                with open(os.path.join(EXPERIMENTS_DIR, file), 'r') as f:
                    results = json.load(f)
                for key, value in results.items():
                    if value is not None:
                        fields_data[key] = fields_data.get(key, []) + [(num_experiment, value)]

        for field, data in fields_data.items():
            if field != 'rounds_means_times':
                sorted_data = sorted(data, key=lambda x: x[0])
                plottable_list = [val for _, val in sorted_data]
                x_values = [val for val, _ in sorted_data]
                plt.figure(figsize=(10, 6))

                plt.plot(x_values, plottable_list, label=field)
                plt.xlabel(x_label)
                plt.ylabel(field.replace('_', ' ').title())
                plt.title(f'Experiment on {field.replace("_", " ").title()}')
                plt.legend()
                plt.grid(True)
                plt.show()


def main():
    visualization = {}
    if len(sys.argv) == 1:
        visualization['experiment'] = sys.argv[1]
    if len(sys.argv) > 1:
        key = ''
        if sys.argv[1].startswith('infected'):
            key = 'numInfected'
        elif sys.argv[1].startswith('speed'):
            key = 'infectionSpeed'
        elif sys.argv[1].startswith('numProc'):
            key = 'numProcesses'
        else:
            print("Invalid argument, please use one of the following: infected, speed, numProc")
            exit(1)
        visualization[key] = int(sys.argv[1].split('-')[1])
        if sys.argv[2].startswith('infected'):
            key = 'numInfected'
        elif sys.argv[2].startswith('speed'):
            key = 'infectionSpeed'
        elif sys.argv[2].startswith('numProc'):
            key = 'numProcesses'
        else:
            print("Invalid argument, please use one of the following: infected, speed, numProc")
            exit(1)
        visualization[key] = int(sys.argv[2].split('-')[1])

    visualize_results(visualization)


if __name__ == '__main__':
    main()
