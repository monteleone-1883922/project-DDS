import os
import shutil

# Definisci il percorso della directory results
results_dir = './results'
experiments_dir = os.path.join(results_dir, 'experiments')

# Verifica che la directory results esista
if os.path.exists(results_dir):
    # Elimina tutto tranne la cartella experiments
    for item in os.listdir(results_dir):
        item_path = os.path.join(results_dir, item)
        if item_path != experiments_dir:
            if os.path.isdir(item_path):
                shutil.rmtree(item_path)  # Elimina la cartella e il suo contenuto
            else:
                os.remove(item_path)  # Elimina il file
                
os.makedirs('./results/merged')
