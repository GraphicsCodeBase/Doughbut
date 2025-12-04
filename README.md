# Doughbut
Doughnut in openGL cause i have free will

# Doughnut Engine - Mathematical Reference

## Overview

This document details the mathematical foundations and implementation of a 3D rotating torus (doughnut) renderer using OpenGL 3.3. The project is inspired by [a1k0n's ASCII donut](https://www.a1k0n.net/2011/07/20/donut-math.html) but implemented with modern 3D graphics.

---

## Table of Contents

1. [Torus Geometry](#torus-geometry)
2. [Vertex Generation](#vertex-generation)
3. [Normal Calculation](#normal-calculation)
4. [Index Generation](#index-generation)
5. [Rotation Mathematics](#rotation-mathematics)
6. [Camera System](#camera-system)
7. [Lighting Model](#lighting-model)
8. [Shader Pipeline](#shader-pipeline)

---

## Torus Geometry

### What is a Torus?

A torus (doughnut shape) is created by rotating a circle around an axis that doesn't intersect the circle.

**Key Parameters:**
- **R** (Major Radius): Distance from the torus center to the tube center
- **r** (Minor Radius): Radius of the tube itself

**Visual Representation:**
```
        ___-------___
    _--             --_
   /     ___---___     \
  |   _-         -_   |  <- Tube (radius r)
  |  /             \  |
  | |       O       | |  <- Center
  |  \_           _/  |
  |   -_       _-   |
   \     ---___---     /
    --_             _--
       ---_______---

  |<------- R ------->|  Major Radius
```

### Parametric Equations

A point on the torus surface is defined by two angles:
- **θ (theta)**: Angle around the main ring [0, 2π]
- **φ (phi)**: Angle around the tube [0, 2π]

**Position equations:**
```
x = (R + r·cos(φ)) · cos(θ)
y = (R + r·cos(φ)) · sin(θ)
z = r · sin(φ)
```

**Breaking it down:**
1. `r·cos(φ)` - Distance from tube center to point on tube
2. `R + r·cos(φ)` - Total distance from origin to point
3. `cos(θ)` and `sin(θ)` - Rotate that distance around the Z-axis
4. `r·sin(φ)` - Height (Z coordinate) based on position around tube

---

## Vertex Generation

### Grid Structure

Vertices are generated in a 2D grid pattern:
- **Major segments**: Number of slices around the ring
- **Minor segments**: Number of slices around the tube

```
Grid indices (i, j):
    j=0    j=1    j=2    j=3   ... j=minorSegments
i=0  •------•------•------•         •
     |      |      |      |         |
i=1  •------•------•------•         •
     |      |      |      |         |
i=2  •------•------•------•         •
     |      |      |      |         |
...
i=majorSegments
```

### Implementation

```cpp
for (unsigned int i = 0; i <= majorSegments; ++i)
{
    float theta = (float)i / (float)majorSegments * 2.0f * PI;
    float cosTheta = cos(theta);
    float sinTheta = sin(theta);

    for (unsigned int j = 0; j <= minorSegments; ++j)
    {
        float phi = (float)j / (float)minorSegments * 2.0f * PI;
        float cosPhi = cos(phi);
        float sinPhi = sin(phi);

        // Calculate position
        float x = (majorRadius + minorRadius * cosPhi) * cosTheta;
        float y = (majorRadius + minorRadius * cosPhi) * sinTheta;
        float z = minorRadius * sinPhi;

        vertex.position = glm::vec3(x, y, z);
    }
}
```

**Why `<= segments`?**
We need one extra vertex to close the loop (vertex at 360° = vertex at 0°).

---

## Normal Calculation

### Surface Normal Theory

The normal at any point on the torus points **perpendicular to the surface**, away from the tube's center axis.

For a torus, the normal is **independent of the major radius R** - it only depends on the local curvature of the tube.

### Normal Equations

```
normal.x = cos(φ) · cos(θ)
normal.y = cos(φ) · sin(θ)
normal.z = sin(φ)
```

Then normalize: `normal = normalize(normal)`

### Why These Equations?

The normal points from the tube's center (at distance R from origin) outward to the surface point. Since the tube is circular, the direction is determined entirely by the angle φ around the tube.

### Implementation

```cpp
glm::vec3 normal = glm::normalize(glm::vec3(
    cosPhi * cosTheta,
    cosPhi * sinTheta,
    sinPhi
));
```

---

## Index Generation

### Quad to Triangle Conversion

Vertices form a grid of quads. Each quad is split into 2 triangles.

```
Quad vertices:
    v0 ---------- v1
    |           / |
    |         /   |
    |       /     |
    |     /       |
    |   /         |
    | /           |
    v2 ---------- v3

Triangle 1: v0 → v2 → v1
Triangle 2: v2 → v3 → v1
```

### Index Calculation

For grid position (i, j), the vertex index is:
```
index = i × (minorSegments + 1) + j
```

### Implementation

```cpp
for (unsigned int i = 0; i < majorSegments; ++i)
{
    for (unsigned int j = 0; j < minorSegments; ++j)
    {
        unsigned int first = i * (minorSegments + 1) + j;
        unsigned int second = first + minorSegments + 1;

        // First triangle
        indices.push_back(first);
        indices.push_back(second);
        indices.push_back(first + 1);

        // Second triangle
        indices.push_back(second);
        indices.push_back(second + 1);
        indices.push_back(first + 1);
    }
}
```

**Winding order matters!** Counter-clockwise winding ensures correct face culling and lighting.

---

## Rotation Mathematics

### Three-Axis Rotation

The doughnut rotates around all three axes simultaneously, inspired by [a1k0n's donut](https://www.a1k0n.net/2011/07/20/donut-math.html).

### Rotation Matrices

**Rotation around X-axis (pitch):**
```
    [1      0           0     ]
Rx = [0   cos(α)   -sin(α)    ]
    [0   sin(α)    cos(α)    ]
```

**Rotation around Y-axis (yaw):**
```
    [ cos(β)    0    sin(β)   ]
Ry = [   0       1      0     ]
    [-sin(β)    0    cos(β)   ]
```

**Rotation around Z-axis (roll):**
```
    [cos(γ)  -sin(γ)    0     ]
Rz = [sin(γ)   cos(γ)    0     ]
    [  0         0       1     ]
```

### Combined Rotation

The model matrix applies rotations in sequence:
```
M = Rz · Ry · Rx
```

**Order matters!** Different orders produce different results (this is due to matrix multiplication being non-commutative).

### Implementation

```cpp
// Update angles
rotationAngleX += rotationSpeedX * deltaTime;
rotationAngleY += rotationSpeedY * deltaTime;
rotationAngleZ += rotationSpeedZ * deltaTime;

// Apply rotations
glm::mat4 model = glm::mat4(1.0f);
model = glm::rotate(model, glm::radians(rotationAngleX), glm::vec3(1, 0, 0));
model = glm::rotate(model, glm::radians(rotationAngleY), glm::vec3(0, 1, 0));
model = glm::rotate(model, glm::radians(rotationAngleZ), glm::vec3(0, 0, 1));
```

### Rotation Speeds

Current configuration:
- **X-axis (pitch)**: 50°/second - Forward/backward tilt
- **Y-axis (yaw)**: 30°/second - Left/right spin
- **Z-axis (roll)**: 70°/second - Side-to-side roll

Different speeds create a complex, non-repeating tumbling pattern.

---

## Camera System

### Camera Components

The camera system consists of:
- **Position**: Camera location in 3D space
- **Target**: Point the camera looks at
- **Direction**: Normalized vector from position to target
- **Up vector**: Defines camera orientation (usually [0, 1, 0])

### View Matrix

The view matrix transforms world coordinates to camera space:
```
V = lookAt(position, target, up)
```

GLM implementation:
```cpp
viewMatrix = glm::lookAt(position, target, up);
```

### Projection Matrix

Perspective projection creates depth perception:
```
P = perspective(fov, aspectRatio, near, far)
```

Where:
- **fov**: Field of view in degrees (70° by default)
- **aspectRatio**: Width / Height
- **near**: Near clipping plane (0.01)
- **far**: Far clipping plane (1000.0)

### Camera Controls

**Movement:**
- Direction vectors: `forward`, `right`, `up`
- Speed: 4.0 units/sec (8.0 with Shift)
- Frame-independent: `position += direction × speed × deltaTime`

**Rotation:**
- Mouse delta × sensitivity = rotation angle
- Applied to camera direction vector
- Uses rotation matrices around side and up axes

---

## Lighting Model

### Phong Lighting

The rendering uses Phong shading with three components:

**1. Ambient Lighting**
```glsl
vec3 ambient = ambientStrength × objectColor;
```
- Constant base lighting (30% strength)
- Simulates indirect/environmental light

**2. Diffuse Lighting**
```glsl
float diff = max(dot(normal, lightDir), 0.0);
vec3 diffuse = diff × objectColor;
```
- Based on angle between surface normal and light direction
- Brighter when surface faces light directly
- Uses Lambert's cosine law

**3. Specular Lighting**
```glsl
vec3 reflectDir = reflect(-lightDir, normal);
float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
vec3 specular = specularStrength × spec × vec3(1.0);
```
- Creates bright highlights
- Depends on view angle relative to reflection
- Power of 32 = sharp, shiny surface

**Final Color:**
```glsl
vec3 result = ambient + diffuse + specular;
```

### Normal Matrix

Normals must be transformed correctly when the model is scaled or rotated non-uniformly:

```cpp
mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
```

This ensures normals remain perpendicular to the surface after transformation.

---

## Shader Pipeline

### Vertex Shader

**Inputs:**
- `position` (vec3): Vertex position in model space
- `normal` (vec3): Surface normal in model space

**Uniforms:**
- `model`: Model matrix (local → world)
- `view`: View matrix (world → camera)
- `projection`: Projection matrix (camera → clip)
- `normalMatrix`: Normal transformation matrix

**Outputs:**
- `gl_Position`: Clip-space vertex position
- `fragPos`: World-space position for lighting
- `fragNormal`: World-space normal for lighting

**Process:**
```glsl
// Transform position
gl_Position = projection × view × model × vec4(position, 1.0);

// Pass world-space position
fragPos = vec3(model × vec4(position, 1.0));

// Transform normal
fragNormal = normalize(normalMatrix × normal);
```

### Fragment Shader

**Inputs:**
- `fragPos`: World-space position
- `fragNormal`: World-space normal

**Uniforms:**
- `lightPos`: Light position in world space
- `viewPos`: Camera position in world space
- `objectColor`: Base color of object

**Output:**
- `FragColor`: Final RGBA color

**Process:**
```glsl
// Calculate lighting vectors
vec3 norm = normalize(fragNormal);
vec3 lightDir = normalize(lightPos - fragPos);
vec3 viewDir = normalize(viewPos - fragPos);

// Ambient
vec3 ambient = 0.3 × objectColor;

// Diffuse
float diff = max(dot(norm, lightDir), 0.0);
vec3 diffuse = diff × objectColor;

// Specular
vec3 reflectDir = reflect(-lightDir, norm);
float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
vec3 specular = 0.5 × spec × vec3(1.0);

// Combine
FragColor = vec4(ambient + diffuse + specular, 1.0);
```

---

## Transformation Pipeline

### Complete Vertex Transformation

A vertex goes through multiple coordinate spaces:

1. **Model Space** (Local coordinates)
   - Vertex as defined in Mesh

2. **World Space** (Model matrix)
   ```
   worldPos = model × localPos
   ```
   - Object positioned/rotated in world

3. **View Space** (View matrix)
   ```
   viewPos = view × worldPos
   ```
   - Relative to camera

4. **Clip Space** (Projection matrix)
   ```
   clipPos = projection × viewPos
   ```
   - Perspective applied

5. **Normalized Device Coordinates** (Perspective divide)
   ```
   ndc = clipPos.xyz / clipPos.w
   ```
   - Range [-1, 1]

6. **Screen Space** (Viewport transform)
   - Final pixel coordinates

### Combined MVP Matrix

Often computed as single matrix:
```
MVP = projection × view × model
gl_Position = MVP × vec4(position, 1.0);
```

---

## Performance Considerations

### Vertex Count

For a torus with `M` major segments and `m` minor segments:
- **Vertices**: `(M + 1) × (m + 1)`
- **Triangles**: `M × m × 2`
- **Indices**: `M × m × 6`

**Example (50 × 30):**
- Vertices: 51 × 31 = 1,581
- Triangles: 50 × 30 × 2 = 3,000
- Indices: 9,000

### Optimizations

1. **Static Mesh**: Generated once, uploaded to GPU
2. **Index Buffer**: Reduces vertex duplication
3. **Precomputed Normals**: Calculated during generation
4. **Matrix Updates**: Only model matrix changes per frame
5. **VSync**: Limits frame rate to monitor refresh (60 FPS)

---

## Controls Reference

### Camera Movement
Hold **Right Mouse Button** and:
- **W** - Move forward
- **S** - Move backward
- **A** - Strafe left
- **D** - Strafe right
- **Space** - Move down
- **Left Ctrl** - Move up
- **Left Shift** - Move faster (2× speed)
- **Mouse Movement** - Look around

### Parameters

**Torus:**
- Major Radius: 1.0
- Minor Radius: 0.4
- Major Segments: 50
- Minor Segments: 30

**Rotation:**
- X-axis: 50°/second
- Y-axis: 30°/second
- Z-axis: 70°/second

**Camera:**
- FOV: 70°
- Near plane: 0.01
- Far plane: 1000.0
- Movement speed: 4.0 units/sec (8.0 with shift)

**Lighting:**
- Light position: (2, 2, 2)
- Object color: (0.3, 0.6, 1.0) - Light blue
- Ambient strength: 0.3
- Specular strength: 0.5
- Shininess: 32

---

## References

### Inspiration
- [a1k0n's Donut Math](https://www.a1k0n.net/2011/07/20/donut-math.html) - Original ASCII donut
- Classic obfuscated C code: `donut.c`

### Mathematics
- Parametric surfaces and torus equations
- 3D rotation matrices and Euler angles
- Phong reflection model
- View and projection transformations

### Libraries Used
- **GLFW**: Window management and input
- **GLAD**: OpenGL function loader
- **GLM**: Mathematics library (vectors, matrices)
- **OpenGL 3.3**: Graphics API

---

## Equations Summary

### Torus Generation
```
x = (R + r·cos(φ)) · cos(θ)
y = (R + r·cos(φ)) · sin(θ)
z = r · sin(φ)

normal.x = cos(φ) · cos(θ)
normal.y = cos(φ) · sin(θ)
normal.z = sin(φ)
```

### Rotation
```
angle(t) = angle₀ + speed × t
```

### Lighting
```
ambient = kₐ × color
diffuse = kd × max(N · L, 0) × color
specular = ks × max(R · V, 0)^n

final = ambient + diffuse + specular
```

### Transformation
```
gl_Position = projection × view × model × position
```

---

**Document Version**: 1.0
**Last Updated**: 2025-12-04
**Engine**: Doughnut 3D Renderer

