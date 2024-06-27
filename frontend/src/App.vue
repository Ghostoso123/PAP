<script setup>
import { useRouter } from 'vue-router';
const router = useRouter();
</script>

<template>
  <div class="h-screen w-screen bg-stone-100">
    <div @click="router.push({ path: '/' })" class="top h-16 sm:h-18 md:h-20 lg:h-24 w-screen bg-white  p-4 fixed shadow-md cursor-pointer">
      <h1 class="text-2xl sm:text-4xl md:text-5xl lg:text-6xl font-semibold pl-4">Estação Metereológica ESDS</h1>
    </div>
    <router-view class="pt-24 h-full w-full bg-gradient-to-br from-zinc-100 to-stone-200"/>
    <footer class="flex justify-center p-6 bg-gray-800 w-screen text-neutral-100">
      <div class="grow">
        <h2 class="text-xl py2">Estação Metereológica ESDS</h2>
        <p class="block">
          <span class="italic">Feito por: <span class="font-bold">Tiago Costa</span> e <span class="font-bold">Alexandre Sousa</span> </span><br>
          <span class="italic">Prof. Acompanhante: <span class="font-bold">João Marques</span> </span><br>
          <span class="text-sm">
            Esta estação meteorológica desenvolvida por nós, Tiago Costa e Alexandre Sousa,
            alunos do curso "Técnico de Eletrónica, Automação e Computadores" frequentado no ano letivo de 2021 a 2023
            teve o objetivo de tornar a nossa PAP (Prova de Aptidão Profissional)
            mais inovadora e criativa usando o mínimo de recursos possíveis e aplicando os conhecimentos adquiridos ao longo de todo o curso.
          </span>
        </p>
      </div>
      <img class="h-36 object-contain" src="/src/assets/esds.png">
    </footer>
  </div>
</template>

