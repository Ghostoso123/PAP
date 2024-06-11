<template>
    <div class="container mx-auto px-4">
      <h1 class="mt-4 text-4xl font-bold"> {{ title }}</h1>
      <table class="table-auto w-full mt-5">
        <thead>
          <tr>
            <th class="px-4 py-2" @click="sortBy('sensor')">Sensor</th>
            <th class="px-4 py-2" @click="sortBy('timestamp')">Timestamp</th>
            <th class="px-4 py-2" @click="sortBy('type')">Type</th>
            <th class="px-4 py-2" @click="sortBy('value')">Value</th>
          </tr>
        </thead>
        <tbody>
          <tr v-for="item in sortedData" :key="item.timestamp">
            <td class="border px-4 py-2">{{ item.sensor }}</td>
            <td class="border px-4 py-2">{{ visualDate(item.timestamp)}}</td>
            <td class="border px-4 py-2">{{ item.type }}</td>
            <td class="border px-4 py-2">{{ item.value }}</td>
          </tr>
        </tbody>
      </table>
      <div class="pagination flex justify-center p-2">
        <button @click="fetchData(pagination.currentPage - 1)" :disabled="pagination.currentPage <= 1">Prev</button>
        <span class="mx-2">{{ pagination.currentPage }}</span>
        <button @click="fetchData(pagination.currentPage + 1)" :disabled="!pagination.nextPage">Next</button>
      </div>
    </div>
  </template>
  
  <script setup>
  import { useRoute, useRouter } from 'vue-router';
  import { ref, computed, onMounted, reactive } from 'vue';

  const props = defineProps({
    type: {
      type: String,
      required: true,
    },
  });

  const route = useRoute();
  const router = useRouter();

  const type = route.params.type;
  
  const sensorData = ref([]);
  const pagination = reactive({ currentPage: 1, nextPage: null });
  const sortKey = ref('');
  const sortOrder = ref('asc');
  
  async function fetchData(page = 1) {
    try {
      const response = await fetch(`${import.meta.env.VITE_API_URL}/weather-data/${type}?page=${page}`, {
          headers: {
            "Accept": "*/*",
          }
      }).catch((error) => {
          console.error('Error:', error);
      }).then(r => r.json());
      console.log('res:', response)
      sensorData.value = response.data;
      pagination.nextPage = response.nextPage ? parseInt(response.nextPage.split('=')[1], 10) : null;
      pagination.currentPage = page;
    } catch (error) {
      console.error(error);
      router.push({ path: '/'})
    }
  }
  
  function sortBy(key) {
    if (sortKey.value === key) {
      sortOrder.value = sortOrder.value === 'asc' ? 'desc' : 'asc';
    } else {
      sortKey.value = key;
      sortOrder.value = 'asc';
    }
  }
  
  const sortedData = computed(() => {
    return sensorData.value.sort((a, b) => {
      if (a[sortKey.value] < b[sortKey.value]) return sortOrder.value === 'asc' ? -1 : 1;
      if (a[sortKey.value] > b[sortKey.value]) return sortOrder.value === 'asc' ? 1 : -1;
      return 0;
    });
  });

  const title = computed(() => {
    return type.charAt(0).toUpperCase() + type.slice(1);
  });

  const visualDate = (date) => {
    return new Date(date).toLocaleString();
  }
  
  onMounted(() => {
    fetchData(pagination.currentPage);
  });
  </script>
  
  <style scoped>
  .pagination button[disabled] {
    opacity: 0.5;
    cursor: not-allowed;
  }
  </style>
  