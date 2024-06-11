<script setup>
import { useRouter, useRoute } from 'vue-router'
import router from '../router';

import Icon from './Icon.vue';

const props = defineProps({
  entry:{
    type: Object,
    required: true
  }
})

const goToHistory = function(){
  console.log(props.entry.type)
  router.push({
    path: `/history/${props.entry.type}`
  })
}
</script>
<template lang="">
  <div class="m-2 p-4 rounded-lg bg-blue-50 border flex flex-row">
    <div class="p-2">
      <Icon :iconType="entry.type" :size="12" :extraClasses="'fill-blue-500'"/>
    </div>
    <div class="flex flex-col p-1">
      <h4 class="text-blue-900">
        {{ entry.label }}
      </h4>
      <h2>
        <span class="text-blue-950 text-3xl"> {{ entry.value }} </span>
        <span class="text-blue-950 text-xl ml-1"> {{ entry.valueLabel }} </span>
      </h2>
    </div>
    <span class="grow">
    </span>
    <div @click="goToHistory" class="p-2 cursor-pointer">
      <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="
        w-12 h-12
        text-slate-600 
      ">
        <path stroke-linecap="round" stroke-linejoin="round" d="M13.5 4.5L21 12m0 0l-7.5 7.5M21 12H3" />
      </svg>

    </div>
  </div>
</template>
