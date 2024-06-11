import { createRouter, createWebHashHistory } from "vue-router";

import Dashboard from '../components/Dashboard.vue'
import History from '../components/History.vue'


const routes = [
    { 
        path: '/', 
        name: 'Dashboard',
        component: Dashboard,
    },
    {
        path: '/history/:type',
        name: 'History',
        component: History,
    },
]

const router = createRouter({
    history: createWebHashHistory(),
    routes,
})

export default router;
