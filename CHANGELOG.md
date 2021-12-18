# DeepSea Track Changelog

## [1.17.5](http://bitbucket.org/mbari/deepsea-track/compare/v1.17.4...v1.17.5) (2021-12-18)


### Bug Fixes

* correct location for ACK ([114123c](http://bitbucket.org/mbari/deepsea-track/commits/114123cc0e8949fc5b4181c89417495e5dddff12))

## [1.17.4](http://bitbucket.org/mbari/deepsea-track/compare/v1.17.3...v1.17.4) (2021-12-17)


### Performance Improvements

* switch to zmq client/server model, 1-based frame from detector, and release frame if no detections ([ab3fbd6](http://bitbucket.org/mbari/deepsea-track/commits/ab3fbd6ac9e5d3defb54905a096ac29b515ec202))

## [1.17.3](http://bitbucket.org/mbari/deepsea-track/compare/v1.17.2...v1.17.3) (2021-12-17)


### Bug Fixes

* correct zmq frame indexing ([d5a49fc](http://bitbucket.org/mbari/deepsea-track/commits/d5a49fc33549f4d5ef399a988e06f4b9a25a8c79))

## [1.17.2](http://bitbucket.org/mbari/deepsea-track/compare/v1.17.1...v1.17.2) (2021-12-14)


### Bug Fixes

* disconnect subscriber on exit ([eff17f1](http://bitbucket.org/mbari/deepsea-track/commits/eff17f1eb72c3e69bbd7bef1ad3473c746abaa28))

## [1.17.1](http://bitbucket.org/mbari/deepsea-track/compare/v1.17.0...v1.17.1) (2021-12-14)


### Bug Fixes

* correct frame number cast to int and check ([cd64a8d](http://bitbucket.org/mbari/deepsea-track/commits/cd64a8d32078f77dfd7fa19d6405c1adfe49c185))
* remove blocking code ([2bcccd4](http://bitbucket.org/mbari/deepsea-track/commits/2bcccd4ca9ee85ee89ddb80e8a79af0c3f8d8def))


### Performance Improvements

* experimental rework of zmq ([94c4453](http://bitbucket.org/mbari/deepsea-track/commits/94c44533ef66af8a90037218da3ca87bffef5e90))
* removed tracker wait ([1c75d19](http://bitbucket.org/mbari/deepsea-track/commits/1c75d1912b3647003e7d57f0de786ed2209520a1))

# [1.17.0](http://bitbucket.org/mbari/deepsea-track/compare/v1.16.3...v1.17.0) (2021-12-05)


### Features

* initial check-in ([12ec2bb](http://bitbucket.org/mbari/deepsea-track/commits/12ec2bb62430062c81b4f5b8bec1a4142fa78096))


### Performance Improvements

* dynamic template sizing and other minor memory mgmt improvements ([e7ac867](http://bitbucket.org/mbari/deepsea-track/commits/e7ac867a1e2945454cbc5778f719e49b5791414b))
* dynamic template sizing and other minor memory mgmt improvements ([524692f](http://bitbucket.org/mbari/deepsea-track/commits/524692fe9a9bd0eac5e4a363b221417a41f4206f))

## [1.16.3](http://bitbucket.org/mbari/deepsea-track/compare/v1.16.2...v1.16.3) (2021-11-22)


### Performance Improvements

*  added fast tracker wrapper, optional gamma for speed-up, removed unused code, log programs args, removed MEDIANFLOW/TLD/MOSSE trackers. ([63b89e1](http://bitbucket.org/mbari/deepsea-track/commits/63b89e1df15b09a75f36b5919ec9893f25f5da40))

## [1.16.2](http://bitbucket.org/mbari/deepsea-track/compare/v1.16.1...v1.16.2) (2021-11-05)


### Performance Improvements

* skip over loop if no detections, objects and not generating visuals ([617768e](http://bitbucket.org/mbari/deepsea-track/commits/617768e7c6ba425d3df20b8860035f68f2be4463))

## [1.16.1](http://bitbucket.org/mbari/deepsea-track/compare/v1.16.0...v1.16.1) (2021-07-15)


### Performance Improvements

* move listening to outside of loop ([c633c40](http://bitbucket.org/mbari/deepsea-track/commits/c633c400af960df6ac7c878ac2b1db04c4b23849))

# [1.16.0](http://bitbucket.org/mbari/deepsea-track/compare/v1.15.0...v1.16.0) (2021-06-16)


### Features

* tracking improvements ([492e9de](http://bitbucket.org/mbari/deepsea-track/commits/492e9de9b9d4fd50490ef6900f5f2449cda65de4))

# [1.15.0](http://bitbucket.org/mbari/deepsea-track/compare/v1.14.0...v1.15.0) (2021-06-08)


### Bug Fixes

* pass through float instead of TrackerType for score and nms ([adbcbe5](http://bitbucket.org/mbari/deepsea-track/commits/adbcbe59ff2d3ab81ab2062c7ce5302aa974e08a))


### Features

* only short first 7 characters of uuid for brevity and print exact path to cfg ([12ceaf9](http://bitbucket.org/mbari/deepsea-track/commits/12ceaf9beff786917853c1745607dafb0c3f3e29))

# [1.14.0](http://bitbucket.org/mbari/deepsea-track/compare/v1.13.1...v1.14.0) (2021-06-04)


### Features

* added option to create video and prefix with video file stem ([e6fe3ab](http://bitbucket.org/mbari/deepsea-track/commits/e6fe3ab37b1fc8670d993b2c06e6662992b38e67))

## [1.13.1](http://bitbucket.org/mbari/deepsea-track/compare/v1.13.0...v1.13.1) (2021-06-01)


### Bug Fixes

* zmq boxes normalized to 0-1 float ([b713c87](http://bitbucket.org/mbari/deepsea-track/commits/b713c87d7ced29d868291890d2c14560d23a2e3d))

# [1.13.0](http://bitbucket.org/mbari/deepsea-track/compare/v1.12.0...v1.13.0) (2021-06-01)


### Features

* added configuration of nms and score thresholds to tracker ([9286dfc](http://bitbucket.org/mbari/deepsea-track/commits/9286dfc9337e322b66d81b5638086711b4def5d3))

# [1.12.0](http://bitbucket.org/mbari/deepsea-track/compare/v1.11.0...v1.12.0) (2021-06-01)


### Features

* added output scaling and better renaming to distinguish resize ([4145dd0](http://bitbucket.org/mbari/deepsea-track/commits/4145dd03f78a5fb42e200c4059ac228eea3abeb9))

# [1.11.0](http://bitbucket.org/mbari/deepsea-track/compare/v1.10.0...v1.11.0) (2021-05-31)


### Features

* added zmq message tracker delay ([be8a424](http://bitbucket.org/mbari/deepsea-track/commits/be8a4240fa176ea5142d533b4836a1cb3c857e1b))

# [1.10.0](http://bitbucket.org/mbari/deepsea-track/compare/v1.9.3...v1.10.0) (2021-05-28)


### Features

* added tracker delay and delay to zmq initialize wait message block ([89bb3f0](http://bitbucket.org/mbari/deepsea-track/commits/89bb3f07fb7ac84bbfc7688efff9e37744578295))

## [1.9.3](http://bitbucket.org/mbari/deepsea-track/compare/v1.9.2...v1.9.3) (2021-05-26)


### Bug Fixes

* correct warning ([77e1d2e](http://bitbucket.org/mbari/deepsea-track/commits/77e1d2e8cd43c42b2a0f6b2c348242e3c19bc4a6))

## [1.9.2](http://bitbucket.org/mbari/deepsea-track/compare/v1.9.1...v1.9.2) (2021-05-24)


### Bug Fixes

* correct path ([9c7d668](http://bitbucket.org/mbari/deepsea-track/commits/9c7d668506880a45e4f9b5779f195be8ff34344e))

## [1.9.1](http://bitbucket.org/mbari/deepsea-track/compare/v1.9.0...v1.9.1) (2021-05-24)


### Bug Fixes

* correct arg ([8185def](http://bitbucket.org/mbari/deepsea-track/commits/8185defca591843c851a704b144fa08a7fd74d67))

# [1.9.0](http://bitbucket.org/mbari/deepsea-track/compare/v1.8.0...v1.9.0) (2021-05-24)


### Features

* split configuration and video input into separate arguments ([f695694](http://bitbucket.org/mbari/deepsea-track/commits/f6956941b20fc74d2dfc39f9fe9c6d84a86cf1d2))

# [1.8.0](http://bitbucket.org/mbari/deepsea-track/compare/v1.7.0...v1.8.0) (2021-04-14)


### Bug Fixes

* add default confidence ([fd23bd5](http://bitbucket.org/mbari/deepsea-track/commits/fd23bd593e877b0382aa2ee8086f1233cb4a09a7))


### Features

* reduce score threshold to 0.6 ([b6b1cf7](http://bitbucket.org/mbari/deepsea-track/commits/b6b1cf7107f63fdf60449f819b48250fae260225))

# [1.7.0](http://bitbucket.org/mbari/deepsea-track/compare/v1.6.0...v1.7.0) (2020-11-19)


### Features

* added index from 1 ([d9d5933](http://bitbucket.org/mbari/deepsea-track/commits/d9d593331248973b1b3f6efae47bb3601b3e2bf0))

# [1.6.0](http://bitbucket.org/mbari/deepsea-track/compare/v1.5.0...v1.6.0) (2020-11-19)


### Features

* zmq and args added ([4e6a851](http://bitbucket.org/mbari/deepsea-track/commits/4e6a8511e17a47f39a13afedb8a57b55b22e37b2))

# [1.5.0](http://bitbucket.org/mbari/deepsea-track/compare/v1.4.1...v1.5.0) (2020-11-19)


### Features

* added tests ([f846835](http://bitbucket.org/mbari/deepsea-track/commits/f8468356edcc17039e2870113b00e6eb6f3b8400))

## [1.4.1](http://bitbucket.org/mbari/deepsea-track/compare/v1.4.0...v1.4.1) (2020-11-19)


### Bug Fixes

* correct download ([5ca745f](http://bitbucket.org/mbari/deepsea-track/commits/5ca745fd1d6deb327422b1fad75f84e8b590bd11))

# [1.4.0](http://bitbucket.org/mbari/deepsea-track/compare/v1.3.0...v1.4.0) (2020-11-18)


### Features

* added all arguments ([326e2df](http://bitbucket.org/mbari/deepsea-track/commits/326e2dfb2354c4246f7c69a71ec439a476062627))
* added example output ([ed58536](http://bitbucket.org/mbari/deepsea-track/commits/ed5853629a58ce8dd754c3b61d09b32ffb476729))
* added second tracker and min frame of 1 as default ([8529ed5](http://bitbucket.org/mbari/deepsea-track/commits/8529ed5301ef92310fb678435b53e9765559ce68))
* added second tracker as default ([20d596a](http://bitbucket.org/mbari/deepsea-track/commits/20d596ae778fb0812a2c2eb046725783e5fa7574))
* removed unused code, initialized logic and more descriptive output ([1da7c93](http://bitbucket.org/mbari/deepsea-track/commits/1da7c93ff3de574a2a6f49e558848a4932e55d2b))
* skip edge check on first frame ([2421260](http://bitbucket.org/mbari/deepsea-track/commits/2421260a032bc9e1dca318a678d31d916d1bd162))

# [1.3.0](http://bitbucket.org/mbari/deepsea-track/compare/v1.2.1...v1.3.0) (2020-11-18)


### Features

* added zma listener ([46c0cdd](http://bitbucket.org/mbari/deepsea-track/commits/46c0cdd184b24a9688c9af2207291e4751546746))

## [1.2.1](http://bitbucket.org/mbari/deepsea-track/compare/v1.2.0...v1.2.1) (2020-10-15)


### Bug Fixes

* correct xerces install and disable onnx for now ([baf5ec3](http://bitbucket.org/mbari/deepsea-track/commits/baf5ec3e38881c02f379e172c34775bb551097dd))

# [1.2.0](http://bitbucket.org/mbari/deepsea-track/compare/v1.1.0...v1.2.0) (2020-10-02)


### Features

* width/height scaling and rescale output to same dimension as input ([2425b88](http://bitbucket.org/mbari/deepsea-track/commits/2425b88c40b77d80305a9eacf030d1a4265fe72e))


### Performance Improvements

* change pad check from 10% to 1% ([ead483c](http://bitbucket.org/mbari/deepsea-track/commits/ead483c86a4dfaf1ff76cbb3fbd18229c2c1e3be))

# [1.1.0](http://bitbucket.org/mbari/deepsea-track/compare/v1.0.3...v1.1.0) (2020-10-01)


### Features

* initial check-in ([c3370d3](http://bitbucket.org/mbari/deepsea-track/commits/c3370d399d48a60fcb2ce4337af2531b00b72c0a))

## [1.0.3](http://bitbucket.org/mbari/deepsea-track/compare/v1.0.2...v1.0.3) (2020-09-25)


### Bug Fixes

* added missing docker command ([1657f33](http://bitbucket.org/mbari/deepsea-track/commits/1657f33ceea9f9394c87d6a6d2bea63e79104ca7))

## [1.0.2](http://bitbucket.org/mbari/deepsea-track/compare/v1.0.1...v1.0.2) (2020-09-20)


### Bug Fixes

* added in missing closure ([3794612](http://bitbucket.org/mbari/deepsea-track/commits/3794612b4d87878634c8be9f2c6499b9698aee41))

## [1.0.1](http://bitbucket.org/mbari/deepsea-track/compare/v1.0.0...v1.0.1) (2020-09-06)


### Bug Fixes

* **tests:** correct test names(untested) ([302d920](http://bitbucket.org/mbari/deepsea-track/commits/302d92070498e0e5326875fb444ab73c032efa45))
