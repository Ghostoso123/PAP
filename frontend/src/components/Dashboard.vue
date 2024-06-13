<script setup>
import Card from './Card.vue'
import { ref } from 'vue'
const data = ref([])

async function getData(){
    const newData = await fetch(`${import.meta.env.VITE_API_URL}/weather-data/latest`, {
        headers: {
      "Accept": "*/*",
        }
    }).then(res => res.json()).catch((error) => {
        console.error('Error:', error);
    });
    const mostRecentByType = {};
    newData.data.map((entry)=>{
        if(entry.type=="temperature"){
            return {
                type: entry.type,
                label: "Temperatura",
                value: entry.value,
                valueLabel: "Cº"
            }
        }
        if(entry.type=="humidity"){
            return {
                type: entry.type,
                label: "Humidade",
                value: entry.value,
                valueLabel: "%"
            }
        }
        if(entry.type=="pressao" || entry.type=="pressure"){
            return {
                type: entry.type,
                label: "Pressão",
                value: entry.value,
                valueLabel: "hPa"
            }
        }
        if(entry.type=="altitude"){
            return {
                type: entry.type,
                label: "Altitude",
                value: entry.value,
                valueLabel: "m"
            }
        }
        if(entry.type=="windspeed"){
            return {
                type: entry.type,
                label: "Velocidade do Vento",
                value: entry.value,
                valueLabel: "m/s"
            }
        }
        if(entry.type=="airquality"){
            return {
                type: entry.type,
                label: "Qualidade do Ar",
                value: entry.value,
                valueLabel: ""
            }
        }
        if(entry.type=="winddirection"){
            return {
                type: entry.type,
                label: "Direção do Vento",
                value: entry.value,
                valueLabel: ""
            }
        }
        console.log("There seems to be a data type that the front does not  accept");
    }).filter((entry)=> entry != undefined && entry != null)
    .forEach(element => {
        const { type, timestamp } = element;

        if (!mostRecentByType[type] || new Date(timestamp) > new Date(mostRecentByType[type].timestamp)) {
            mostRecentByType[type] = element;
        }
    });

    data.value = Object.values(mostRecentByType);
}

getData()

</script>
<template lang="">
    <div class="flex justify-center p-4">
        <div class="w-1/2 flex flex-col mt-24">
            <Card v-for="entry in data" :entry="entry" class="m-4"/>
        </div>
    </div>
</template>
