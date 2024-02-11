FROM nodered/node-red:3.1-debian

USER root

# Mise à jour du cache et installation de fontconfig et ffmpeg
RUN apt-get update && \
    apt-get install -y fontconfig ffmpeg && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

USER node-red

