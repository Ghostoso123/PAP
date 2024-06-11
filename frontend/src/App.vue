<script setup>
import { useRouter } from 'vue-router';
const router = useRouter();
</script>

<template>
  <div class="h-screen w-screen bg-stone-100">
    <div @click="router.push({ path: '/' })" class="top h-24 w-screen bg-white p-4 fixed shadow-md cursor-pointer">
      <h1 class="text-6xl font-semibold">Estação Metereológica</h1>
    </div>
    <router-view class="pt-24 h-full w-full"/>
  </div>
</template>
