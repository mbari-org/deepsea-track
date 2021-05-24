# DeepSea Track Changelog

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
