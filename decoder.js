// IoT-Anywhere Coopster - Chicken Coop Monitor
// Example payload: 6400f9272e0292019102004B

function Decoder(payload, port) {
        var decoded = [
            {
                field: "HUMIDITY",
                value: (payload[0] / 2.0) // Unit: %
            },
            {
                field: "TEMPERATURE",
                value: ((payload[1] << 8) | payload[2]) / 10.0 // Unit: C
            },
            {
                field: "PRESSURE",
                value: ((payload[3] << 8) | payload[4]) * 10
            },
            {
                field: "GAS",
                value: ((payload[5] << 8) | payload[6]) * 10
            },
            {
                field: "BATTERY",
                value: (payload[7] << 8 | payload[8]) / 100 // Unit: V
            },
            {
                field: "AI_COUNT",
                value: payload[9]
            },
            {
                field: "AI_STATUS",
                value: payload[10]
            },
            {
                field: "AI_AVG_CONFIDENCE",
                value: payload[11]
            }
        ];
        
        //For use with Datacake. Otherwise remove below.
        if (normalizedPayload.gateways && normalizedPayload.gateways[0]) {
            decoded.push(
                {
                    field: "LORA_SNR",
                    value: normalizedPayload.gateways[0].snr
                },        
                {
                    field: "LORA_RSSI",
                    value: normalizedPayload.gateways[0].rssi
                },        
                {
                    field: "LORA_DATARATE",
                    value: normalizedPayload.data_rate
                }              
            );        
        }
        
        return decoded;
}