{
    "Source" : "SkyGaussianBlur.azsl",
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
            "name": "GaussianBlurPS",
            "type" : "Fragment"
        }
        ]
    }
}
