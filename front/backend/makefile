RUN = npm run dev
BUILD = npm run build
HOST = root@106.14.140.93
DOCKER_BUILD = docker-compose up -d --build

all: clean deploy
clean:
	npm cache clean -f
	rm -rf node_modules
deploy: scp docker
scp:
	scp -r * $(HOST):/root/front/
docker:
	ssh $(HOST) 'cd front && $(DOCKER_BUILD)'
