// Copyright 2021 The WebNN-native Authors
//
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

#include "examples/ResNet/ResNet.h"
#include "src/tests/WebnnTest.h"

class ResNetNchwTests : public WebnnTest {
  public:
    void TestResNetNchw(const std::string& inputFile, const std::string& expectedFile) {
        const ml::GraphBuilder builder = ml::CreateGraphBuilder(GetContext());
        ResNet resnet;
        const std::string nchwPath =
            "node/third_party/webnn-polyfill/test-data/models/resnet50v2_nchw/";
        ml::Graph graph = resnet.LoadNCHW(nchwPath + "weights/", false);
        const cnpy::NpyArray inputNpy = cnpy::npy_load(nchwPath + "test_data_set/" + inputFile);
        const std::vector<float> inputData = inputNpy.as_vec<float>();
        std::vector<float> result(utils::SizeOfShape({1, 1000}));
        utils::Compute(graph, {{"input", inputData}}, {{"output", result}});
        const cnpy::NpyArray outputNpy = cnpy::npy_load(nchwPath + "test_data_set/" + expectedFile);
        EXPECT_TRUE(utils::CheckValue(result, outputNpy.as_vec<float>()));
    }
};

TEST_F(ResNetNchwTests, NchwTest0) {
    TestResNetNchw("0/input_0.npy", "0/output_0.npy");
}

TEST_F(ResNetNchwTests, NchwTest1) {
    TestResNetNchw("1/input_0.npy", "1/output_0.npy");
}

TEST_F(ResNetNchwTests, NchwTest2) {
    TestResNetNchw("2/input_0.npy", "2/output_0.npy");
}
