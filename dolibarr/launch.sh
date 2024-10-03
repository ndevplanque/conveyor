#!/bin/bash

cd "$(dirname "$0")"

docker compose down
docker compose up --detach --build

echo "Launched Dolibarr"
