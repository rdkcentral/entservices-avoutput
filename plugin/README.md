# entservices-avoutput
repo which contains AVOutput Thunder plugin functionality

## Release MD5 Checksum Documentation

Use MD5 checksums for release artifact integrity validation.

### 1. Generate Release Artifact

From the repository root, generate a source archive for a release tag:

TAG="<release-tag>"   # Example: 1.2.3
ARTIFACT="entservices-avoutput-${TAG}.tar.gz"
git archive --format=tar.gz --output "${ARTIFACT}" "${TAG}"

### 2. Generate MD5 File

Create a companion checksum file:

md5sum "${ARTIFACT}" > "${ARTIFACT}.md5"

### 3. Verify MD5 Checksum

Consumers can verify the downloaded artifact using:

md5sum -c "${ARTIFACT}.md5"

Expected output on success:

entservices-avoutput-<release-tag>.tar.gz: OK

### 4. Release Publishing Guidance

For each release, publish both files together:

- `entservices-avoutput-<release-tag>.tar.gz`
- `entservices-avoutput-<release-tag>.tar.gz.md5`

Include a short note in the release description indicating that checksum verification can be performed with `md5sum -c`.
