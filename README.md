# LTC-based-sheen-models

This project presents two sheen models that can approximate the appearance of cloth-like surfaces in OpenGL based on the concepts presented in the paper "Practical Multiple-Scattering Sheen Using Linearly Transformed Cosines" by Zeltner et al. The paper highlights the challenges in replicating the appearance of the fibrous surfaces of fabrics and explains the two traditional methods to create the sheen effect. The proposed models utilise linearly transformed cosines (LTC) to provide a fast and accurate rendering of complex lighting effects. The first implementation also uses a precomputed lookup table for the sheen, while the second implementation obtains the cosine value for each fragment and applies a continuous function.

## Video demo

[![Watch the demo](https://img.youtube.com/vi/560lCUEFmLg/0.jpg)](https://www.youtube.com/watch?v=560lCUEFmLg)

## Background

This project aims to build upon the concepts presented in the paper "Practical Multiple-Scattering Sheen Using Linearly Transformed Cosines" [[1]](#references) by Tizian Zeltner, Brent Burley, and Matt Jen-Yuan Chiang, published in SIGGRAPH 2022 and authored by researchers from l’École Polytechnique Fédérale de Lausanne (EPFL) and Disney Animation Studios. Based on the motivations and concepts of that paper, this project has produced two sheen models that can approximate the appearance of cloth-like surfaces in OpenGL.

The surfaces of many fabrics are characterised by fibres that protrude from their surface, such as the intentional nap of velvet or the stray fibres of wool. These fibres impact the material's visual appearance [[1]](#references). However, replicating the same appearance is not straightforward. Traditionally, two methods have been employed to create the sheen effect these fibres produce: simulating and rendering thousands of individual fibres in semi-random directions or using a sheen model. The former approach is expensive and can result in undesired reflectance, making the latter option the preferred method. However, previous sheen models utilised by Disney did not adequately account for backward scattering, leading to subtle but noticeable discrepancies in the appearance of certain materials compared to their real-life counterparts. As a result, the improvements to the sheen models proposed by this paper have real implications for the industry, as they allow for a better approximation of the complex visual properties of fibrous surfaces.

This paper presents a novel BRDF sheen model to accurately approximate surfaces covered in normally-oriented fibres. The proposed model utilises linearly transformed cosines (LTC) to achieve this effect. Linearly Transformed Cosines (LTC) is a physically based method for achieving real-time polygonal-light shading, introduced in the research article titled "Real-Time Polygonal-Light Shading with Linearly Transformed Cosines" by E. Heitz et al. [[2]](#references) in 2016. The LTC method employs precomputed cosine functions representing the amount of light emanating from a specific direction. During rendering, the appropriate cosine function is selected for each fragment based on its normal and the light source direction. These cosine functions are then transformed linearly using a matrix, enabling various visual effects, including soft shadows, specularity, and scattering. This approach allows LTC to provide a fast and accurate rendering of complex lighting effects, making it a valuable tool for real-time graphics applications.

## Implementation details

In this project, the cloth models have been generated using the cloth simulation feature of Blender. This technique allowed us to create realistic models. Additionally, the disposition of the scene has been intentionally designed as a homage to the original scene used by Disney. By incorporating similar spatial and lighting arrangements, we captured the essence of the original scene.

Two sheen model implementations were produced in this project. The first implementation follows the principles introduced by Zeltner et al., whereas the second implementation is a custom cosine-based method.

The first method implemented employs a precomputed 32x32 lookup table provided by the authors in the paper's git repository [[3]](#references) to obtain appropriate coefficients based on the cosine of the angle of reflection on the surface. This is done using an alpha parameter, and the shader evaluates the LTC distribution in its default coordinate system to determine the reflectance value. The resulting float value is then clamped between 0 and 1 to ensure it falls within the valid range. Next, this method computes the sheen effect using the LTC model, which considers the surface normal and reflectance values along with the given surface and light direction. These values are combined with a Csheen parameter, determining the final sheen. Finally, the resulting sheen layer is added to the conventional LTC-light value to obtain the final appearance of the fragment.

The LTC code in the fragment shader was implemented using reference code from Github and blog posts [[4]](#references)[[5]](#references)[[6]](#references). Pre-fitted coefficients were obtained from Joey de Vries' Github repository and adapted to the existing OpenGL base pipeline used for other assignments of this module. The coefficients were loaded into arrays and passed to OpenGL as images, allowing for efficient interpolation and computing in the fragment shader.

In developing the sheen model, the code provided by Tizian Zeltner was utilised as a reference implementation for sampling and testing the model in PBRT [[7]](#references). The code was adapted in OpenGL to suit the specific pipeline requirements. Due to differences in the data and variables used by PBRT and OpenGL, the implementation required the computation of equivalent versions of these variables in the fragment shader. Additionally, several of the built-in functions of PBRT were implemented in GLSL to produce comparable results. Notably, the original implementation utilised arrays and manually interpolated the coefficients. In contrast, the produced implementation followed the approach taken in the LTC model by passing the data as a texture to the fragment shader.

![Comparison](https://github.com/borjagq/LTC-based-sheen-models/blob/main/Results/LTC-Sheen.png?raw=true)

The second sheen model does not use linearly transformed cosines or precomputed coefficients to compute each fragment's sheen level. Instead, it obtains the cosine value for each fragment and normalises them to the 0-1 range. Then, a continuous function f(a,b) maps the unit interval [0,1] onto itself. The function is defined by the equation f(a,b) = (1 - exp(-b * a)) / (1 - exp(-b)), where the exponential function exp(-b * a) controls the rate of decay of the function as an increase from 0 to 1, and the parameter b controls the rate of decay. When a is equal to 0, f(a,b) equals 0, and when a is equal to 1, f(a,b) equals 1. The normalisation of the function ensures that it is continuous and satisfies the initial conditions f(0,b) = 0 and f(1,b) = 1. Then, this layer is added to the base LTC illumination described at the beginning of this section. This method could be more precise regarding local illumination. It only considers the observing point and the fragment's normals but obtains realistic results for globally illuminated scenes when the beta parameter is well adjusted. The chosen function is the following:

$f(\theta, \beta) = \frac{1 - e^{-\beta * \theta}}{1 - e^{-\beta}};$

## Results / evaluation

The visual evaluation shows that both implementations produce sheen effects that seem realistic to the eye, although both obtain noticeably different results. In addition, both implementations allow the simulation of different types of materials by tweaking the parameters.

![Comparison](https://github.com/borjagq/LTC-based-sheen-models/blob/main/Results/ZelComp.png?raw=true)

![Comparison](https://github.com/borjagq/LTC-based-sheen-models/blob/main/Results/CosComp.png?raw=true)

Performance-wise, both implementations can be comfortably handled in real-time by the computer used during the development (Macbook Pro, 2021, Apple M1 8-core CPU, 8-core GPU, 16GB RAM, 3024x1964px display). Moreover, in both cases, the computer can reach its maximum FPS count of 120 (the maximum framerate supported by the display is 120 when running ProRes [[8]](#references)) when rendering the scenes. Using linearly transformed cosines provides a fast and accurate rendering of complex lighting effects, making it a valuable tool for real-time graphics applications. The programme is not costly in terms of computing power, either. It takes around 20% of CPU usage, 42% of GPU usage and 300MB of RAM, which is not a lot for a graphical application.

Furthermore, the original authors demonstrated the applicability of their sheen model to simulate dusty objects. In this project, we extended their approach by modifying the shader to incorporate the Y component of the normal as a multiplier for the sheen effect. As a result, we were able to successfully replicate the simulation of dusty objects in our sewing machine model.

![Comparison](https://github.com/borjagq/LTC-based-sheen-models/blob/main/Results/Sewing.png?raw=true)

## Improvements

While the current implementation of the project has achieved satisfactory results, there are still some areas that could be improved upon in future iterations. One area of concern is the singularity that occurs around the centre of coordinates in the implementation proposed by Zeltner. This issue could be addressed by further studying how PBRT handles vector transformations and exploring alternative approaches to mitigate this problem in OpenGL.

Another potential area for improvement is the second sheen model, which could benefit from more direct interaction with local illumination. While the current approach of direct multiplication can achieve some level of realism, it falls short compared to the appearance of real-life materials such as velvet. By exploring alternative methods that consider the local environment and its effects on the sheen, we may achieve even more convincing and accurate results. These improvements would further enhance the realism and overall quality of the project.

## References

- [1] Zeltner, Tizian, Brent Burley, and Matt Jen-Yuan Chiang (Aug. 2022). “Practical Multiple-Scattering Sheen Using Linearly Transformed Cosines”. In: Special Interest Group on Computer Graphics and Interactive Techniques Conference Talks. Vancouver BC Canada: ACM, pp. 1–2. ISBN: 978-1-4503-9371-3. DOI: [10.1145/3532836.3536240](https://dl.acm.org/doi/10.1145/3532836.3536240) (visited on 05/01/2023).

- [2] Heitz, Eric et al. (July 2016). “Real-time polygonal-light shading with linearly transformed cosines”. In: ACM Transactions on Graphics 35.4, 41:1–41:8. [Link](https://dl.acm.org/doi/10.1145/2897824.2925895) (visited on 05/01/2023).

- [3] Zeltner, Tizian (Oct. 2022). Practical Multiple-Scattering Sheen Using Linearly Transformed Cosines. original-date: 2022-05-24T09:21:04Z. [Link](https://github.com/tizian/ltc-sheen) (visited on 05/01/2023).

- [4] Christensen, Alexander (2022). LearnOpenGL - Area Lights. [Link](https://learnopengl.com/Guest-Articles/2022/Area-Lights) (visited on 03/28/2023).

- [5] Hill, Stephen (Apr. 2023). selfshadow/ltc code. original-date: 2017-05-31T23:46:37Z. [Link](https://github.com/selfshadow/ltc_code) (visited on 05/01/2023).

- [6] Vries, Joey de (May 2023). learnopengl.com code repository. original-date: 2015-03-23T14:21:27Z. [Link](https://github.com/JoeyDeVries/LearnOpenGL) (visited on 05/01/2023).

- [7] Physically Based Rendering: From Theory to Implementation (2023). [Link](https://pbrt.org/) (visited on 05/01/2023).

- [8] MacBook Pro 14- and 16-inch - Technical Specifications (2023). [Link](https://www.apple.com/by/macbook-pro-14-and-16/specs/) (visited on 05/01/2023).
