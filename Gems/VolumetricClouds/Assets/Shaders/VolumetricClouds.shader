{
    "Source" : "VolumetricClouds.azsl",
    "DepthStencilState" : {
        "Depth": 
        {
            "Enable": false,  //required to bind depth buffer SRV
            "CompareFunc" : "Always"
        }
    },
    "DrawList": "forward",
    "GlobalTargetBlendState": {
        "Enable": true,
        "BlendSource": "One",
        "BlendDest": "AlphaSourceInverse",
        "BlendOp": "Add"
    },
    "ProgramSettings": {
        "EntryPoints": [
        {
            "name": "MainVS",
            "type" : "Vertex"
        },
        {
            "name": "MainPS",
            "type" : "Fragment"
        }
        ]
    }
}
