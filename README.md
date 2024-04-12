# Vision

We are going to write an engine for creating 3D games, which will help people create compelling experiences. 

When creating games, developers face a lot of problems. Including graphic visualization, animations, sound, moving objects and characters, their actions in accordance with scripts, as well as built-in graphic scenes, compliance with physical effects and laws, and much more.

The solutions to such problems are always about the same and quite difficult for high-quality implementation, so they create game engines that save developers with a layer of abstraction.

Our engine will include:

- A library for writing convenient and understandable gameplay scripts
- Facilities to quickly bootstrap your game enabling fast prototyping
- Built-in profiling and logging to help you find bottlenecks and fix logic errors offline

To demonstrate its abilities we are going to create a game. First person view, explore the immersive world around you.

Player is a magician who travels between multiple locations(something like buildings, temples, towers, ruins, castles, dungeons).

In each location there are monsters which can be killed and puzzles which can be solved using spells. After completing everything inside one location, the player gets a new spell or some upgrade (more damage or modification for spells he already knew) and gets teleported to another location.

# Documentation

### Text Rendering
Text class object is storing one exact font information with exact one font size. This font size is used for preparing textures by Freefont library, so to use another font you have to completele update Object. Despite this you have opportunity to render different size text with one Text Object. For it we have scale parameter in RenderObject. But the farther the scale from 1.f the worse quality of letters because of scaling. 

Probably later it will be stored as object component, but have to think better about it.

To render text on the screen you need create Text object with font in .ttf format after the initialization of glfw. Then you can call text.RenderObject method to set coordinates of text, scale and color.


** CAREFUL, COORDINATES ARE RELATIVE NOW, AS IN IMAGES!!!** 

```C++
auto textOcra = new Font("OCRAEXT.TTF", 20); // Create new Font object
auto fpsObj = new FpsText(); // create new fpsObj
fpsObj->text = new Text(textOcra, "", 0.8, 0.8, 1.f, Vec3(0, 0, 0)); // set new text component to fpsObj
engine.AddObject<>(fpsObj); // add to objects list
```

### Image Rendering
```C++
Object* healthBar1 = new Object();
healthBar1->image = new Image("hp.png", 0.03, 0.15, 0.4);
engine.AddObject<>(healthBar1);
```

To render image see code above and *Image.hpp*. 

+ Coordinates are relative, range [0; 1]. 
+ Coodrinates starts in left bottom corner in both image and screen.
+ Image have to be in resources/images folder.

#### Probably bugs and problems
+ No game-window: Text objects can be created only after glfw initialization


### SkeletalAnimations

SkeletalAnimation component contains vector of SkeletalAnimationData. You can add animationData even after initialization, using method AddAnimation.
To get method Play(int id, bool looped) id you can call function GetAnimationsInfo - it's formatted string showing all available animations.
It's avalible not to work with SkeletalAnimationData, because all methods have equivalents using filenames.

```C++
{
    // /* PIGEON */
    Model *pigeonModel = Model::loadFromFile("pigeon/scene.gltf");
    pigeonModel->shader = skeletalShaderProgram;

    auto pigeonAnimation = new SkeletalAnimationData("pigeon/scene.gltf", 0, pigeonModel);

    auto pigeonObj = engine.NewObject();
    pigeonObj.AddTransform(Vec3(0.f, -10.f, -10.f), Vec3(10.f), Mat4(1.0));
    pigeonObj.AddModel(*pigeonModel);
    pigeonObj.AddSkeletalAnimationsManager(pigeonAnimation).PlayImmediately(0, 1);
}

{
    /* Wolf */
    Model *wolfModel = Model::loadFromFile("Wolf/Wolf-Blender-2.82a.gltf");
    wolfModel->meshes.pop_back();
    wolfModel->meshes.erase(wolfModel->meshes.begin() + 1);  // Delete floor
    wolfModel->shader = skeletalShaderProgram;

    auto wolfObj = engine.NewObject();
    wolfObj.AddTransform(Vec3(5.f, -10.f, -10.f), Vec3(10.f), Mat4(1.0));
    wolfObj.AddModel(*wolfModel);
    auto& wolfManager = wolfObj.AddSkeletalAnimationsManager("Wolf/Wolf-Blender-2.82a.gltf", wolfModel); // Написать что обязательно нужно &
    wolfManager.PlayImmediately(0, 0);
    Logger::Info("%s", wolfManager.GetAnimationsInfo().c_str());
}

{
    /* XBot */
    Model *pigeonModel = Model::loadFromFile("XBot/XBot.dae");
    pigeonModel->shader = skeletalShaderProgram;

    auto pigeonAnimation1 = new SkeletalAnimationData("XBot/Praying.dae", 0, pigeonModel);

    auto pigeonObj = engine.NewObject();
    pigeonObj.AddTransform(Vec3(0.f, -10.f, -10.f), Vec3(5.f), Mat4(1.0));
    pigeonObj.AddModel(*pigeonModel);
    auto& animManager = pigeonObj.AddSkeletalAnimationsManager(pigeonAnimation1);
    animManager.AddAnimation("XBot/Hip Hop Dancing.dae", pigeonModel);
    Logger::Info("%s", animManager.GetAnimationsInfo().c_str());
    animManager.PlayImmediately(0, 0);
}
```

+ SkeletalAnimationData have to be created only with new
+ use AddSkeletalAnimationManager return value only with auto&.
+ Currently if you are downloading from mixamo, use option download with skin. 
