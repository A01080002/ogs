#!/usr/bin/env groovy
@Library('jenkins-pipeline@master') _

pipeline {
  agent none
  options {
    ansiColor('xterm')
    timestamps()
  }
  stages {
    stage('Build') {
      parallel {
        // ************************** Docker ***********************************
        stage('Docker') {
          agent {
            docker {
              image 'ogs6/gcc-gui:latest'
              label 'docker'
              args '-v /home/jenkins/.ccache:/usr/src/.ccache'
              alwaysPull true
            }
          }
          environment {
            CONTENTFUL_ACCESS_TOKEN = credentials('CONTENTFUL_ACCESS_TOKEN')
            CONTENTFUL_OGS_SPACE_ID = credentials('CONTENTFUL_OGS_SPACE_ID')
          }
          steps {
            // Install web dependencies
            script {
              sh("""
                cd web
                yarn --ignore-engines --non-interactive
                node node_modules/node-sass/scripts/install.js
                npm rebuild node-sass
                sudo -H pip install -r requirements.txt
                """.stripIndent())

              configure { cmakeOptions =
                '-DCMAKE_BUILD_TYPE=Release ' +
                '-DOGS_CPU_ARCHITECTURE=generic ' +
                '-DOGS_WEB_BASE_URL=$JOB_URL"Web/" ' // TODO: or '-DOGS_WEB_BASE_URL=https://dev.opengeosys.org'
              }
              build { }
              build { target="tests" }
              build { target="ctest" }
              build { target="web" }
            }
          }
          post {
            always {
              publishReports { }
            }
            failure {
                dir('build') { deleteDir() }
            }
            success {
                stash(name: 'web', includes: 'web')
                stash(name: 'doxygen', includes: 'build/docs')
                script {
                  publishHTML(target: [allowMissing: false, alwaysLinkToLastBuild: true,
                    keepAll: true, reportDir: 'build/docs', reportFiles: 'index.html',
                    reportName: 'Doxygen'])
                  step([$class: 'WarningsPublisher', canResolveRelativePaths: false,
                    messagesPattern: """
                      .*DOT_GRAPH_MAX_NODES.
                      .*potential recursive class relation.*""",
                    parserConfigurations: [[parserName: 'Doxygen', pattern:
                    'build/DoxygenWarnings.log']], unstableTotalAll: '0'])
                }
                dir('build') { deleteDir() }
            }
          }
        }
        // ************************ Docker-Conan *******************************
        stage('Docker-Conan') {
          agent {
            docker {
              image 'ogs6/gcc-conan:latest'
              label 'docker'
              args '-v /home/jenkins/.ccache:/usr/src/.ccache'
              alwaysPull true
            }
          }
          steps {
            script {
              configure {
                cmakeOptions =
                  '-DCMAKE_BUILD_TYPE=Release ' +
                  '-DOGS_USE_CONAN=ON ' +
                  '-DOGS_CONAN_BUILD=never ' +
                  '-DOGS_CPU_ARCHITECTURE=generic ' +
                  '-DOGS_PACKAGE_DEPENDENCIES=ON '
              }
              build { }
              build { target="tests" }
              build { target="ctest" }
              configure {
                cmakeOptions =
                  '-DOGS_BUILD_CLI=OFF ' +
                  '-DOGS_USE_PCH=OFF ' +
                  '-DOGS_BUILD_GUI=ON ' +
                  '-DOGS_BUILD_UTILS=ON ' +
                  '-DOGS_BUILD_TESTS=OFF ' +
                  '-DOGS_BUILD_METIS=ON '
                keepDir = true
              }
              build { }
            }
          }
          post {
            always {
              publishReports { }
            }
            failure {
                dir('build') { deleteDir() }
            }
            success {
                archiveArtifacts 'build/*.tar.gz,build/conaninfo.txt'
                dir('build') { deleteDir() }
            }
          }
        }
        // ************************** Windows **********************************
        stage('Win') {
          agent {
            label "win && conan"
          }
          environment {
            MSVC_NUMBER = '15'
            MSVC_VERSION = "2017"
          }
          steps {
            script {
              // CLI
              configure {
                cmakeOptions =
                  '-DOGS_USE_CONAN=ON ' +
                  '-DOGS_DOWNLOAD_ADDITIONAL_CONTENT=ON '
              }
              build { }
              build { target="tests" }
              build { target="ctest" }
              // GUI
              configure {
                cmakeOptions =
                  '-DOGS_BUILD_GUI=ON ' +
                  '-DOGS_BUILD_UTILS=ON ' +
                  '-DOGS_BUILD_TESTS=OFF ' +
                  '-DOGS_BUILD_SWMM=ON ' +
                  '-DOGS_BUILD_METIS=ON '
                  keepDir = true
              }
              build { }
            }
          }
          post {
            always {
              publishReports { }
            }
            failure {
                dir('build') { deleteDir() }
            }
            success {
                archiveArtifacts 'build/*.zip,build/conaninfo.txt'
                dir('build') { deleteDir() }
            }
          }
        }
      } // end parallel
    } // end stage Build

    // ***************************** Deploy ************************************
    stage('Deploy') {
      when { environment name: 'JOB_NAME', value: 'OpenGeoSys/ogs/master' }
      steps {
        unstash web
        unstash doxygen
        script {
          sshagent(credentials: ['www-data_jenkins']) {
            sh 'rsync -a --delete --stats -e "ssh -o UserKnownHostsFile=' +
               'ogs/scripts/jenkins/known_hosts" ogs/web/public/ ' +
               'www-data@jenkins.opengeosys.org:/var/www/dev.opengeosys.org'
            sh 'rsync -a --delete --stats -e "ssh -o UserKnownHostsFile=' +
               'ogs/scripts/jenkins/known_hosts" build/docs/ ' +
               'www-data@jenkins.opengeosys.org:/var/www/doxygen.opengeosys.org'
          }
        }
      }
    }
  }
}
