<template>
  <div>
    <div class="joystick">
      <div ref="zone" class="zone"></div>
    </div>
    <div>
      force: {{force}}, radian: {{radian}}, degree: {{degree}}
      <br>
      output <strong>left: {{left}}, right: {{right}}</strong>
    </div>
  </div>
</template>

<style>
.joystick {
  position: relative;
  text-align: center;
  height: 375px;
}
.joystick .zone {
  position: absolute;
  left: 50%;
  transform: translateX(-50%);
  background-color: #999;
  width: 375px;
  height: 375px;
}
</style>

<script>
import nipplejs from 'nipplejs'
import mqtt from 'mqtt'

export default {
  name: 'joystick',
  data () {
    return {
      manager: null,
      client: null,
      force: 0,
      radian: 0,
      degree: 0,
      left: 0,
      right: 0,
      prevLeft: 0,
      prevRight: 0
    }
  },
  created: function () {
    this.client = mqtt.connect('mqtt://192.168.1.19:1884')
    this.client.on('connect', () => {
      console.log('MQTT connected!')
      window.setInterval(() => {
        if (this.left !== this.prevLeft || this.right !== this.prevRight) {
          this.client.publish('tank/1goki/move', [this.left, this.right].join(','))
          this.prevLeft = this.left
          this.prevRight = this.right
        }
      }, 50)
    })
  },
  mounted: function () {
    this.manager = nipplejs.create({
      zone: this.$refs.zone,
      mode: 'static',
      position: {top: '50%', left: '50%'},
      multitouch: true,
      size: 125, // 25% of 500px(width)
      fadeTime: 50
    })
    this.manager.on('start end', (evt, data) => {
      this.debug(data)
      this.updateMotor(data)
    }).on('move', (evt, data) => {
      this.debug(data)
      this.updateMotor(data)
    })
  },
  beforeDestroy: function () {
    this.manager.destroy()
    this.client.end()
  },
  methods: {
    debug: function (data) {
      let force = data.force ? data.force.toFixed(2) : 0
      this.force = force > 3 ? 3 : force
      this.radian = data.angle ? data.angle.radian.toFixed(2) : 0
      this.degree = data.angle ? data.angle.degree.toFixed(2) : 0
    },
    updateMotor: function (data) {
      let radian = this.radian
       // first & fourth quadrant
      if (radian <= Math.PI * 0.5 || (radian >= Math.PI * 1.75 && radian <= Math.PI * 2)) {
        this.left = 1
        this.right = Math.sin(radian)
      } else if (radian > Math.PI * 0.5 && radian <= Math.PI * 1.25) {
        this.left = Math.sin(radian)
        this.right = 1
      } else {
        this.left = -1
        this.right = -1
      }
      // } else if (radian > Math.PI && radian <= Math.PI * 3 / 2) {
      //   this.left = Math.sin(radian)
      //   this.right = -1
      // } else {
      //   this.left = -1
      //   this.right = Math.sin(radian)
      // }
      let force = data.force ? data.force.toFixed(2) : 0
      force = force / 3 * 15
      force = force >= 10 ? 10 : force
      this.left = Math.ceil(force * this.left)
      this.right = Math.ceil(force * this.right)
    }
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
h1, h2 {
  font-weight: normal;
}

ul {
  list-style-type: none;
  padding: 0;
}

li {
  display: inline-block;
  margin: 0 10px;
}

a {
  color: #42b983;
}
</style>
