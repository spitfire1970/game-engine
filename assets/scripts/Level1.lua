-- Define a table with the values of the first level
Level = {
    ----------------------------------------------------
    -- Table to define the list of assets
    ----------------------------------------------------
    assets = {
        { type = "texture", id = "tank-image",       file = "./assets/images/tank-panther-right.png" },
        { type = "texture", id = "truck-image",      file = "./assets/images/truck-ford-right.png" },
        { type = "texture", id = "tree-image",       file = "./assets/images/tree.png" },
        { type = "texture", id = "tilemap-image",    file = "./assets/tilemaps/jungle.png" },
        { type = "texture", id = "chopper-image",    file = "./assets/images/chopper-spritesheet.png" },
        { type = "texture", id = "radar-image",      file = "./assets/images/radar.png" },
        { type = "texture", id = "bullet-image",     file = "./assets/images/bullet.png" },
        { type = "font"   , id = "charriot-font-20", file = "./assets/fonts/charriot.ttf", font_size = 20 },
        { type = "font"   , id = "arial-font-8",     file = "./assets/fonts/arial.ttf", font_size = 8 }
    },

    ----------------------------------------------------
    -- table to define the map config variables
    ----------------------------------------------------
    tilemap = {
        map_file = "./assets/tilemaps/jungle.map",
        texture_asset_id = "tilemap-image",
        num_rows = 20,
        num_cols = 25,
        tile_size = 32,
        scale = 1.5
    },

    ----------------------------------------------------
    -- table to define entities and their components
    ----------------------------------------------------
    entities = {
        {
            -- Player chopper
            tag = "player",
            components = {
                transform = {
                    position = { x = 100, y = 200 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 0.0, -- degrees
                },
                rigidbody = {
                    velocity = { x = 20.0, y = 20.0 }
                },
                sprite = {
                    texture_asset_id = "chopper-image",
                    width = 32,
                    height = 32,
                    z_index = 2
                },
                animation = {
                    num_frames = 2,
                    speed_rate = 10 -- fps
                },
                boxcollider = {
                    width = 32,
                    height = 32,
                    offset = { x = 0, y = 0 }
                },
                health = {
                    health_percentage = 100
                },
                keyboard_controller = {
                    up_key = "Up",
                    right_key = "Right",
                    down_key = "Down",
                    left_key = "Left",
                    speed = 100.0
                },
                projectile_emitter = {
                    projectile_velocity = { x = 150, y = 150 },
                    projectile_duration = 5, -- seconds
                    hit_percentage_damage = 10
                    -- no repeat_frequency = never fires automatically
                },
                camera_follow = {
                    follow = true
                },
                text_label = {}
            }
        },
        {
            -- Chopper 2
            components = {
                transform = {
                    position = { x = 500, y = 200 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 0.0, -- degrees
                },
                rigidbody = {
                    velocity = { x = -20.0, y = -20.0 }
                },
                sprite = {
                    texture_asset_id = "chopper-image",
                    width = 32,
                    height = 32,
                    z_index = 2
                },
                animation = {
                    num_frames = 2,
                    speed_rate = 10 -- fps
                },
                boxcollider = {
                    width = 32,
                    height = 32,
                    offset = { x = 0, y = 0 }
                },
                health = {
                    health_percentage = 100
                },
                keyboard_controller = {
                    up_key = "W",
                    right_key = "D",
                    down_key = "S",
                    left_key = "A",
                    speed = 100.0
                }
            }
        },
        {
            -- Tank
            group = "enemies",
            components = {
                transform = {
                    position = { x = 500, y = 200 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 0.0, -- degrees
                },
                rigidbody = {
                    velocity = { x = 20.0, y = 0.0 }
                },
                sprite = {
                    texture_asset_id = "tank-image",
                    width = 32,
                    height = 32,
                    z_index = 2
                },
                boxcollider = {
                    width = 32,
                    height = 32,
                    offset = { x = 0, y = 0 }
                },
                health = {
                    health_percentage = 100
                },
                text_label = {}
            }
        },
        {
            -- Truck
            group = "enemies",
            components = {
                transform = {
                    position = { x = 100, y = 300 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 90.0, -- degrees
                },
                rigidbody = {
                    velocity = { x = 0.0, y = 25.0 }
                },
                sprite = {
                    texture_asset_id = "truck-image",
                    width = 32,
                    height = 32,
                    z_index = 1
                },
                boxcollider = {
                    width = 32,
                    height = 32,
                    offset = { x = 0, y = 0 }
                },
                health = {
                    health_percentage = 100
                },
                projectile_emitter = {
                    projectile_velocity = { x = 0, y = 100 },
                    projectile_duration = 5, -- seconds
                    repeat_frequency = 2,    -- seconds
                    hit_percentage_damage = 10
                },
                text_label = {}
            }
        },
        {
            -- Tree A
            group = "obstacles",
            components = {
                transform = {
                    position = { x = 400, y = 200 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 0.0, -- degrees
                },
                sprite = {
                    texture_asset_id = "tree-image",
                    width = 16,
                    height = 32,
                    z_index = 1
                },
                boxcollider = {
                    width = 14,
                    height = 32,
                    offset = { x = 0, y = 0 }
                }
            }
        },
        {
            -- Tree B
            group = "obstacles",
            components = {
                transform = {
                    position = { x = 600, y = 200 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 0.0, -- degrees
                },
                sprite = {
                    texture_asset_id = "tree-image",
                    width = 16,
                    height = 32,
                    z_index = 1
                },
                boxcollider = {
                    width = 14,
                    height = 32,
                    offset = { x = 0, y = 0 }
                }
            }
        },
        {
            -- Tree C
            group = "obstacles",
            components = {
                transform = {
                    position = { x = 100, y = 200 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 0.0, -- degrees
                },
                sprite = {
                    texture_asset_id = "tree-image",
                    width = 16,
                    height = 32,
                    z_index = 1
                },
                boxcollider = {
                    width = 14,
                    height = 32,
                    offset = { x = 0, y = 0 }
                }
            }
        },
        {
            -- Tree D
            group = "obstacles",
            components = {
                transform = {
                    position = { x = 100, y = 400 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 0.0, -- degrees
                },
                sprite = {
                    texture_asset_id = "tree-image",
                    width = 16,
                    height = 32,
                    z_index = 1
                },
                boxcollider = {
                    width = 14,
                    height = 32,
                    offset = { x = 0, y = 0 }
                }
            }
        },
        {
            -- Radar (fixed to screen)
            components = {
                transform = {
                    position = { x = window_width - 74, y = 10 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 0.0, -- degrees
                    fixed = true
                },
                sprite = {
                    texture_asset_id = "radar-image",
                    width = 64,
                    height = 64,
                    z_index = 1
                },
                animation = {
                    num_frames = 8,
                    speed_rate = 5 -- fps
                }
            }
        },
        {
            -- Title label (fixed to screen)
            components = {
                transform = {
                    position = { x = math.floor(window_width / 2) - 40, y = 10 },
                    fixed = true
                },
                text_label = {
                    text = "CHOPPER 1.0",
                    font_asset_id = "charriot-font-20",
                    color = { r = 0, g = 255, b = 0 }
                }
            }
        }
    }
}