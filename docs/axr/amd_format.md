# Arclight Model File Specification (.amd)

### Preface

The *Arclight Accelerated Renderer (AXR)* is responsible for rendering models. 
Existing model file formats suffer from various issues: Missing or incompatible data, unsupported attributes and tedious parsing.
Since AXR offers its own interface for rendering models, an adaptive file format becomes necessary to fully support the engine's intrinsics.

The AMD file format serves as a solution to this problem.

### Supported features
- Mesh subdivision according to materials
- Mesh hierarchies to allow parent-child relationships for transformations

### File format

**Header**

The header is present in all versions of the .amd file format. Fields from 0x6 onwards may change in any version.

| Offset | Type    | Name        | Description                   |
|--------|---------|-------------|-------------------------------|
| 0x0    | char[4] | magic       | "AMDL" in ASCII               |
| 0x4    | u8      | maj_version | Major version                 |
| 0x5    | u8      | min_version | Minor version                 |
| 0x6    | u16     | mesh_count  | Number of meshes              |
| 0x8    | Mesh*   | root_mesh   | Offset to the root mesh chunk |

**Mesh Chunk**

Mesh chunks define a mesh's attribute layout, hierarchy position and mesh-local settings.

| Offset | Type               | Name                   | Description                     |
|--------|--------------------|------------------------|---------------------------------|
| 0x0    | u32                | vertex_count           | Number of total vertices        |
| 0x4    | u16                | attribute_count        | Number of attributes            |
| 0x6    | u8                 | primitive_mode         | Type of primitive to render     |
| 0x7    | u8                 | indexed                | Set if mesh contains index data |
| 0x8    | AttributeTable*    | attribute_table_offset | Offset to attribute table       |
| 0xC    | void*              | data_offset            | Offset to the data block        |
| 0x10   | u8                 | name_length            | Length of name string           |
| 0x11   | char[name_length]  | name                   | Name of mesh                    |
| +0x0   | Mesh*              | parent                 | Parent mesh                     |
| +0x4   | u32                | child_count            | Child mesh count                |
| +0x8   | Mesh*[child_count] | children               | Child meshes                    |

**Attribute Table**

The attribute table holds information about individual attributes and their layout/offset into the data block.

| Offset | Type | Name      | Description                               |
|--------|------|-----------|-------------------------------------------|
| 0x0    | u8   | attr_type | Type of the attribute                     |
| 0x1    | u8   | data_type | Data type                                 |
| 0x2    | u8   | stride    | Stride (non-zero for interleaved data)    |
| 0x3    | u8   | divisor   | Divisor (non-zero for instanced data)     |
| 0x4    | u32  | size      | Total data size in bytes                  |
| 0x8    | u32  | offset    | Offset into the data block (first vertex) |