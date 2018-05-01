FROM node:carbon
WORKDIR /usr/src/app
COPY ./myWeatherServer/package*.json ./
COPY ./myWeatherServer .
RUN npm install
EXPOSE 3000
CMD [ "npm", "start" ]

