@Version 3.0
@Name TestProgram
@CompileVariants UseNormals, DisableNormals

@Shader
	@Pass Forward
		@Depth Off
		@Stencil On
		@BlendFunction Additive
		@BlendEquation Additive

		@Shared
			struct ShaderData{
				vec3 position;
				vec2 uv;
				vec3 normal;
			};

		@Vertex
			@AttributeSet TextureOnly
			@Attribute Displacement vec3 _vDisplacement;
			@Include axr/simple_shader.asvi

			out ShaderData toFrag;

			void main(){
				vec4 position = vec4(attrPosition, 1.0);
				mat4 modelMatrix = axrGetModelMatrix();
				
				@VariantOnly UseNormals
					mat3 normalMatrix = axrGetNormalMatrix();
				@EndVariant
				
				toFrag.position = _ViewMatrix * modelMatrix * position;
				toFrag.uv = attrUV;
				toFrag.normal = normalMatrix * attrNormal;
				
				_vPosition = _ProjectionMatrix * _ViewMatrix * modelMatrix * position;
				
			}


		@Fragment
			@RenderTargetSet Default
			@Include axr/simple_shader.asfi

			in ShaderData fromVertex;
			uniform sampler2D diffuseTexture;

			void main(){
				vec3 diffuseColor = texture(diffuseTexture, fromVertex.uv);
				_fColor = axrBlinnPhong(fromVertex.position, fromVertex.normal);
			}

	@Pass ForwardX

		@Depth LessEqual
		@Stencil Off
		@BlendFunc Subtractive

		@Vertex
			@Include axr/simple_shader.asv

		@Fragment
			@Include axr/simple_shader.asf