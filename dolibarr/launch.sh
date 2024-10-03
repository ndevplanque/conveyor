#!/bin/bash

cd "$(dirname "$0")"

# activate if you want to delete the container before launching
# docker compose down
docker compose up --detach --build

echo "Launched Dolibarr"
