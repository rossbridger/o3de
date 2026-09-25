{
    "Source" : "SkyRadialBlur.azsl",
    "DepthStencilState" : {
        "Depth": 
        {
            "Enable": false,  //required to bind depth buffer SRV
            "CompareFunc" : "Always"
        }
    },
    "DrawList": "forward",
    "GlobalTargetBlendState": {
        "Enable": false
    },
    "ProgramSettings": {
        "EntryPoints": [
        {
            "name": "MainVS",
            "type" : "Vertex"
        },
        {
            "name": "RadialBlurPS",
            "type" : "Fragment"
        }
        ]
    }
}
