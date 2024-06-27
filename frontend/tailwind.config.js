/** @type {import('tailwindcss').Config} */
export default {
  content: [
    "./index.html",
    "./src/**/*.{vue,js,ts,jsx,tsx}",
  ],
  theme: {
    extend: {
      'height': {
				'18': '4.5rem'
      }
    },
  },
  plugins: [],
}

