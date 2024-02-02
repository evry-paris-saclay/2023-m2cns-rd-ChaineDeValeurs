import requests
import os

def download_file(url, save_path):
    print("Tentative de téléchargement depuis:", url)
    response = requests.get(url, stream=True)
    total_size = int(response.headers.get('content-length', 0))
    block_size = 1024  # 1 KB
    progress_bar_width = 40
    downloaded_size = 0

    print("Taille totale du fichier:", total_size)

    with open(save_path, 'wb') as f:
        for data in response.iter_content(block_size):
            f.write(data)
            downloaded_size += len(data)
            progress = min(int(downloaded_size * progress_bar_width / total_size), progress_bar_width)
            percent = downloaded_size * 100 / total_size
            status = f"[{'=' * progress}{' ' * (progress_bar_width - progress)}] {percent:.2f}%"
            print(status, end='\r', flush=True)

    print("\nTéléchargement terminé.")

url = "https://zenodo.org/records/7643417/files/Medical%20Waste%204.0.zip?download=1"
save_path = "Medical_Waste_4.0.zip"  # Vous pouvez spécifier le chemin et le nom du fichier de sortie ici

download_file(url, save_path)
