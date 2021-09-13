// Copyright 2021 The WebNN-native Authors

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "src/tests/WebnnTest.h"

class ClampTests : public WebnnTest {
  public:
    void TestClamp(const std::vector<int32_t>& inputShape,
                   const std::vector<float>& inputData,
                   const std::vector<float>& expectedValue,
                   const ml::ClampOptions* options = nullptr) {
        const ml::GraphBuilder builder = ml::CreateGraphBuilder(GetContext());
        const ml::Operand a = utils::BuildInput(builder, "a", inputShape);
        const ml::Operand b = builder.Clamp(a, options);
        const ml::Graph graph = utils::Build(builder, {{"b", b}});
        ASSERT_TRUE(graph);
        std::vector<float> result(utils::SizeOfShape(inputShape));
        utils::Compute(graph, {{"a", inputData}}, {{"b", result}});
        EXPECT_TRUE(utils::CheckValue(result, expectedValue));
    }
};

TEST_F(ClampTests, Clamp) {
    ml::ClampOptions options = {-5, 5};
    TestClamp({3}, {-1, 0, 1}, {-1, 0, 1}, &options);
    TestClamp({3}, {-6, 0, 6}, {-5, 0, 5}, &options);
    TestClamp({3}, {-1, 0, 6}, {-1, 0, 5}, &options);

    options = {-1, 1};
    TestClamp({3}, {-2, 0, 2}, {-1, 0, 1}, &options);
    TestClamp(
        {3, 4, 5},
        {0.58585083,  1.1363881,   0.67161655, -0.9741674,  -1.6196846,  0.572627,   1.9026182,
         -0.7756641,  -0.18808974, -1.0357478, 1.1778295,   -2.305167,   -2.2636602, 0.3750199,
         -0.08234365, -0.47962302, -0.3010948, 0.5369879,   -0.413804,   -1.096925,  -0.9273629,
         0.88833886,  -0.52474195, -1.3852776, 0.10217833,  0.50499475,  1.3289608,  0.21790339,
         -0.65971124, 0.47400787,  0.7271749,  -0.03890531, -0.04459939, 0.2601329,  -0.06985649,
         0.2501139,   -1.0219133,  -1.1504377, -0.83611137, 0.64221096,  0.25879756, 1.040239,
         -0.18669093, -1.1436414,  1.1445535,  -0.01876706, 1.283455,    0.59794647, 2.1886187,
         -0.21977298, 0.90072393,  0.8913641,  -0.55512637, -0.17248231, -1.4617383, -1.5487962,
         0.1265688,   0.7930071,   0.63802403, 0.3400246},
        {0.58585083,  1.,          0.67161655, -0.9741674,  -1.,         0.572627,   1.,
         -0.7756641,  -0.18808974, -1.,        1.,          -1.,         -1.,        0.3750199,
         -0.08234365, -0.47962302, -0.3010948, 0.5369879,   -0.413804,   -1.,        -0.9273629,
         0.88833886,  -0.52474195, -1.,        0.10217833,  0.50499475,  1.,         0.21790339,
         -0.65971124, 0.47400787,  0.7271749,  -0.03890531, -0.04459939, 0.2601329,  -0.06985649,
         0.2501139,   -1.,         -1.,        -0.83611137, 0.64221096,  0.25879756, 1.,
         -0.18669093, -1.,         1.,         -0.01876706, 1.,          0.59794647, 1.,
         -0.21977298, 0.90072393,  0.8913641,  -0.55512637, -0.17248231, -1.,        -1.,
         0.1265688,   0.7930071,   0.63802403, 0.3400246},
        &options);
}

TEST_F(ClampTests, ClampWithDefaults) {
    TestClamp({3}, {-1, 0, 1}, {-1, 0, 1});
    ml::ClampOptions options;
    options.minValue = 0;
    TestClamp(
        {3, 4, 5},
        {0.86301714, -0.5896978,  -0.27253276, 0.7375215,   0.43311873,  -0.21018882, 1.3207943,
         -1.2920012, -0.51867867, -0.28339776, 0.8165349,   0.0023852,   -1.2614918,  0.5140042,
         1.0875463,  0.73930454,  0.61915493,  -1.8743135,  -0.8998865,  0.4820806,   -0.05488819,
         0.5225576,  -1.2663426,  -0.06149476, -1.389781,   -1.9536786,  0.29577908,  0.8425888,
         0.24561642, -0.03299648, -1.5620143,  1.0061071,   -0.0440449,  1.9595621,   0.9423143,
         -2.0051255, 0.7550497,   -1.3965353,  -0.7594955,  -0.25075668, -0.09406245, 0.39756522,
         -1.022855,  -1.150692,   0.6006052,   -0.01325027, 0.17437305,  -2.1936834,  -0.17713739,
         -0.8907292, -0.9206264,  0.9219348,   -1.0956712,  -1.0928966,  -0.3310106,  0.45028883,
         -0.8840147, 1.2341441,   1.4498476,   -0.8814471},
        {0.86301714, 0.,         0.,         0.7375215, 0.43311873, 0.,         1.3207943,
         0.,         0.,         0.,         0.8165349, 0.0023852,  0.,         0.5140042,
         1.0875463,  0.73930454, 0.61915493, 0.,        0.,         0.4820806,  0.,
         0.5225576,  0.,         0.,         0.,        0.,         0.29577908, 0.8425888,
         0.24561642, 0.,         0.,         1.0061071, 0.,         1.9595621,  0.9423143,
         0.,         0.7550497,  0.,         0.,        0.,         0.,         0.39756522,
         0.,         0.,         0.6006052,  0.,        0.17437305, 0.,         0.,
         0.,         0.,         0.9219348,  0.,        0.,         0.,         0.45028883,
         0.,         1.2341441,  1.4498476,  0.},
        &options);

    options = {};
    options.maxValue = 0;
    TestClamp(
        {3, 4, 5},
        {-0.24508175, -0.7786755,  -1.6853821,  0.30301106,  0.7335949,   2.0118642,   -0.8974095,
         1.336235,    1.3423537,   0.19785331,  0.6021635,   0.8732731,   1.9741,      0.47780856,
         -0.06013789, -0.8661688,  0.30532077,  1.0241649,   0.24461035,  -0.77992326, 0.08907621,
         -0.12915348, 0.26473877,  -1.6618484,  0.55078864,  0.59542316,  0.44485343,  -0.00376282,
         -1.8059362,  -0.01932279, 1.060715,    -0.8601289,  -1.9892695,  -1.540558,   0.3140257,
         0.37287602,  0.8862932,   -0.055259,   -1.5003284,  -0.81850415, 0.8188394,   0.14049591,
         0.6498296,   0.4347888,   -0.20496055, -0.17400683, 1.8571023,   0.41467425,  -0.12858754,
         0.45542,     0.22290581,  -2.1573563,  0.6500845,   1.8209393,   -0.7802799,  1.4540358,
         -0.2568697,  0.2934714,   1.0703601,   -0.72000146},
        {
            -0.24508175, -0.7786755,  -1.6853821,  0.,          0.,         0.,
            -0.8974095,  0.,          0.,          0.,          0.,         0.,
            0.,          0.,          -0.06013789, -0.8661688,  0.,         0.,
            0.,          -0.77992326, 0.,          -0.12915348, 0.,         -1.6618484,
            0.,          0.,          0.,          -0.00376282, -1.8059362, -0.01932279,
            0.,          -0.8601289,  -1.9892695,  -1.540558,   0.,         0.,
            0.,          -0.055259,   -1.5003284,  -0.81850415, 0.,         0.,
            0.,          0.,          -0.20496055, -0.17400683, 0.,         0.,
            -0.12858754, 0.,          0.,          -2.1573563,  0.,         0.,
            -0.7802799,  0.,          -0.2568697,  0.,          0.,         -0.72000146,
        },
        &options);
}