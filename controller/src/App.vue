<template>
  <div id="app">
    <h2>MQTT Tank Controller</h2>
    <joystick></joystick>
  </div>
</template>

<script>
import Joystick from './components/Joystick'

export default {
  name: 'app',
  components: {
    Joystick
  }
}
</script>

<style>
#app {
  font-family: 'Avenir', Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
  margin-top: 20px;
}
body {
  padding: 0;
  margin: 0;
}
</style>
