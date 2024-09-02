import sys
import os
import json
import matplotlib.pyplot as plt

EXPERIMENTS_DIR = 'results/experiments'


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
        fields_data = {}

        # Collect data from all JSON files in the experiments directory
        for file in os.listdir(EXPERIMENTS_DIR):
            if file.endswith(".json") \
                    and (num_infected is None or f'infected-{num_infected}' in file) \
                    and (num_processes is None or f'numProc-{num_processes}' in file) \
                    and (infection_speed is None or f'speed-{infection_speed}' in file):
                with open(os.path.join(EXPERIMENTS_DIR, file), 'r') as f:
                    results = json.load(f)
                for key, value in results.items():
                    fields_data[key] = fields_data.get(key, []) + [value]
        for field, data in fields_data.items():
            plt.figure(figsize=(10, 6))

            plt.plot(data, label=field)
            plt.xlabel('experiment')
            plt.ylabel(field.replace('_', ' ').title())
            plt.title(f'{field.replace("_", " ").title()} per Experiment')
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
