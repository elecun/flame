{
    "md5":"603239CAF2F774498037F944F5592F52",
    "info":{
        "taskname":"cm4ind.adc.reader",
        "version":"0.0.1",
        "cpu_affinity":1,
        "cycle_ns":1000000000,
        "policy":{
            "check_jitter":false,
            "check_overrun":false,
            "fault_level":0
        }
    },
    "configurations":{
        "device":[
            {"name" : "adc_1", "port":"/sys/bus/iio/devices/iio\:device0/in_voltage1_raw"},
            {"name" : "adc_2", "port":"/sys/bus/iio/devices/iio\:device0/in_voltage2_raw"},
            {"name" : "adc_3", "port":"/sys/bus/iio/devices/iio\:device0/in_voltage3_raw"},
            ],

        "mqtt":{
            "broker":"127.0.0.1",
            "port":1883,
            "pub_topic":"mro/adc",
            "pub_qos":0,
            "sub_topic":["mro/control"],
            "keep_alive":60
        }
    }
}